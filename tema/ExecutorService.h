#pragma once

#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>

class ExecutorService {
public:
    explicit ExecutorService(int numThreads);
    ~ExecutorService();

    void submit(std::function<void()> task);
    void wait();

private:
    struct Worker;
    Worker* getWorker();

    std::vector<std::unique_ptr<Worker>> workers_;
    std::atomic<bool> stop_;
    std::atomic<int> tasksRemaining_;
    std::atomic<int> nextIndex_{ 0 };

    std::mutex waitMutex_;
    std::condition_variable waitCv_;
};
