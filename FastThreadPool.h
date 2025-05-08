#pragma once

#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <atomic>
#include <iostream>

class FastThreadPool {
public:
    explicit FastThreadPool(size_t numThreads);
    ~FastThreadPool();

    template<typename Func>
    auto submit(Func task) -> std::future<decltype(task())>;

    void wait();

private:
    void workerLoop(size_t id);

    struct Worker {
        std::mutex mutex;
        std::deque<std::function<void()>> queue;
    };

    std::vector<Worker> workers_;
    std::vector<std::thread> threads_;
    std::vector<std::atomic<int>> taskCounters_;

    std::atomic<bool> stop_ = false;
    std::atomic<size_t> index_ = 0;
    std::atomic<int> tasksInFlight_ = 0;

    std::mutex waitMutex_;
    std::condition_variable cvDone_;
};

template<typename Func>
auto FastThreadPool::submit(Func task) -> std::future<decltype(task())> {
    using ReturnType = decltype(task());
    auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(std::move(task));
    std::future<ReturnType> result = packagedTask->get_future();

    size_t i = index_.fetch_add(1) % workers_.size();
    {
        std::lock_guard<std::mutex> lock(workers_[i].mutex);
        workers_[i].queue.emplace_back([packagedTask]() { (*packagedTask)(); });
    }

    ++tasksInFlight_;
    return result;
}
