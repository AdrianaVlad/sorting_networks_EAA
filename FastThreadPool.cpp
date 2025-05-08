#include "FastThreadPool.h"

FastThreadPool::FastThreadPool(size_t numThreads) : workers_(numThreads), taskCounters_(numThreads) {
    for (auto& counter : taskCounters_) {
        counter = 0;
    }

    for (size_t i = 0; i < numThreads; ++i) {
        threads_.emplace_back([this, i]() { workerLoop(i); });
    }
}

FastThreadPool::~FastThreadPool() {
    stop_ = true;

    for (auto& worker : workers_) {
        std::lock_guard<std::mutex> lock(worker.mutex);
        worker.queue.emplace_back([] {});
    }

    for (auto& t : threads_) {
        if (t.joinable()) t.join();
    }

}

void FastThreadPool::wait() {
    std::unique_lock<std::mutex> lock(waitMutex_);
    cvDone_.wait(lock, [this]() { return tasksInFlight_ == 0; });
}


void FastThreadPool::workerLoop(size_t id) {
    auto& self = workers_[id];
    while (!stop_) {
        std::function<void()> task;

        {
            std::lock_guard<std::mutex> lock(self.mutex);
            if (!self.queue.empty()) {
                task = std::move(self.queue.front());
                self.queue.pop_front();
            }
        }

        if (!task) {
            for (size_t i = 0; i < workers_.size(); ++i) {
                size_t victim = (id + i) % workers_.size();
                if (victim == id) continue;

                auto& other = workers_[victim];
                std::lock_guard<std::mutex> lock(other.mutex);
                if (!other.queue.empty()) {
                    task = std::move(other.queue.back());
                    other.queue.pop_back();
                    break;
                }
            }
        }

        if (task) {
            task();
            taskCounters_[id]++;
            if (--tasksInFlight_ == 0) {
                cvDone_.notify_all();
            }
        }
        else {
            std::this_thread::sleep_for(std::chrono::microseconds(50));
        }
    }
}
