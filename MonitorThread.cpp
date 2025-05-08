#include "MonitorThread.h"
#include "NetworkGenerator.h"
#include "Statistics.h"
#include "Config.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

MonitorThread::MonitorThread(NetworkGenerator* generator)
    : generator_(generator),
    intervalMs_(Config::getMonitorTime() * 1000),
    running_(false)
{
    //std::cout << "[DEBUG] Monitor interval (ms): " << intervalMs_ << std::endl;
}

MonitorThread::~MonitorThread() {
    stop();
}

void MonitorThread::start() {
    if (intervalMs_ <= 0) {
        //std::cout << "[DEBUG] Monitor disabled (interval <= 0)" << std::endl;
        running_ = false;
        return;
    }

    running_ = true;
    monitorThread_ = std::thread(&MonitorThread::run, this);
    monitorThread_.detach();
}

void MonitorThread::stop() {
    running_ = false;
}

void MonitorThread::setRunning(bool running) {
    running_ = running;
}

bool MonitorThread::isRunning() const {
    return running_;
}

void MonitorThread::setFinalCheck() {
    if (!running_) return;

    printInfo();
    finalCheck_ = true;

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &now_c);
#else
    localtime_r(&now_c, &now_c);
#endif
    std::cout << std::put_time(&tm, "%F %T") << "\tfinal check" << std::endl << std::flush;
}

void MonitorThread::run() {
    if (intervalMs_ <= 0) {
        running_ = false;
        return;
    }

    //std::cout << "[DEBUG] MonitorThread started" << std::endl << std::flush;

    while (running_) {
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs_));

        if (running_) {
            printInfo();
        }
    }
}

void MonitorThread::printInfo() {
    auto now = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::time_t now_c = millis / 1000;
    int ms = millis % 1000;

    std::tm local_tm;
#ifdef _WIN32
    localtime_s(&local_tm, &now_c);
#else
    localtime_r(&now_c, &local_tm);
#endif

    std::cout << std::put_time(&local_tm, "%F %T") << "." << std::setfill('0') << std::setw(3) << ms << "\t";

    if (generator_->getWorkList()) {
        std::cout << generator_->getWorkList()->size();
    }
    else {
        std::cout << "(no workList)";
    }

    if (!finalCheck_) {
        if (generator_->getTotalNetworks() > 0) {
            double percent = 100.0 * generator_->getCheckedNetworks() / generator_->getTotalNetworks();
            std::cout << "\t" << static_cast<int>(percent) << "%";
        }
    }

    std::cout << std::endl << std::flush;
}
