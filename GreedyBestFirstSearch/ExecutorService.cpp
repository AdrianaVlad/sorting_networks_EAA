#include "ExecutorService.h"
#include <random>
#include <chrono>

struct ExecutorService::Worker {
    ExecutorService& pool_;
    std::deque<std::function<void()>> queue_;
    std::mutex queueMutex_;
    std::condition_variable cv_;
    std::thread thread_;
    int index_;
    std::mt19937 rng_;

    Worker(ExecutorService& pool, int index)
        : pool_(pool), index_(index), rng_(std::random_device{}()) {}

    void start() {
        thread_ = std::thread([this]() {
            while (!pool_.stop_) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    if (!queue_.empty()) {
                        task = std::move(queue_.back());
                        queue_.pop_back();
                    }
                    else {
                        lock.unlock();
                        if (!stealTask(task)) {
                            std::unique_lock<std::mutex> waitLock(queueMutex_);
                            cv_.wait_for(waitLock, std::chrono::milliseconds(1));
                            continue;
                        }
                    }
                }

                if (task) {
                    task();
                    if (--pool_.tasksRemaining_ == 0) {
                        std::unique_lock<std::mutex> lk(pool_.waitMutex_);
                        pool_.waitCv_.notify_all();
                    }
                }
            }
            });
    }

    void enqueue(std::function<void()> task) {
        {
            std::lock_guard<std::mutex> lock(queueMutex_);
            queue_.push_back(std::move(task));
        }
        cv_.notify_one();
    }

    void notify() {
        cv_.notify_one();
    }

    bool stealTask(std::function<void()>& task) {
        std::uniform_int_distribution<int> dist(0, pool_.workers_.size() - 1);
        for (int i = 0; i < 4; ++i) {
            int victim = dist(rng_);
            if (victim == index_) continue;

            auto& v = pool_.workers_[victim];
            std::unique_lock<std::mutex> lock(v->queueMutex_);
            if (!v->queue_.empty()) {
                task = std::move(v->queue_.front());
                v->queue_.pop_front();
                return true;
            }
        }
        return false;
    }
};

ExecutorService::ExecutorService(int numThreads)
    : stop_(false), tasksRemaining_(0) {
    for (int i = 0; i < numThreads; ++i) {
        workers_.emplace_back(std::make_unique<Worker>(*this, i));
    }
    for (auto& w : workers_) {
        w->start();
    }
}

ExecutorService::~ExecutorService() {
    stop_ = true;
    for (auto& w : workers_) {
        w->notify();
    }
    for (auto& w : workers_) {
        if (w->thread_.joinable()) {
            w->thread_.join();
        }
    }
}

void ExecutorService::submit(std::function<void()> task) {
    tasksRemaining_++;
    getWorker()->enqueue(std::move(task));
}

void ExecutorService::wait() {
    std::unique_lock<std::mutex> lock(waitMutex_);
    waitCv_.wait(lock, [this] { return tasksRemaining_ == 0; });
}

ExecutorService::Worker* ExecutorService::getWorker() {
    static thread_local int localIndex = -1;
    if (localIndex == -1) {
        localIndex = nextIndex_++ % workers_.size();
    }
    return workers_[localIndex].get();
}
