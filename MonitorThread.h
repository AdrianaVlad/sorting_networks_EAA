#pragma once

class NetworkGenerator;
#include <thread>
#include <atomic>
#include <chrono>
#include <string>

class MonitorThread {
private:
    NetworkGenerator* generator_;
    int intervalMs_;
    std::atomic<bool> running_;
    bool finalCheck_ = false;
    std::thread monitorThread_;

    void run();

public:
    explicit MonitorThread(NetworkGenerator* generator);
    ~MonitorThread();

    void start();
    void stop();
    void setRunning(bool running);
    bool isRunning() const;
    void setFinalCheck();
    void printInfo();
};
