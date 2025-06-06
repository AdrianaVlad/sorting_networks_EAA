#pragma once

#include "RuntimeNetwork.h"
#include "WorkingList.h"
#include "MonitorThread.h"
#include "NetworkIO.h"
#include "Statistics.h"
#include "Config.h"
#include "FastThreadPool.h"
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include <shared_mutex>

class NetworkGenerator {
private:
    const int nbWires_;
    const int fromSize_;
    const int toSize_;
    std::vector<std::unique_ptr<RuntimeNetwork>> list_;
    std::unique_ptr<FastThreadPool> threadPool_;
    std::shared_mutex workLock_;
    std::mutex cleanupLock_;
    std::unique_ptr<WorkingList> workList_;
    std::unique_ptr<MonitorThread> monitor_;

    Network* prefix_;
    Network* suffix_;

    std::atomic<long> totalNetworks_{ 0 };
    std::atomic<long> checkedNetworks_{ 0 };

    static inline bool SUBSUMPTION_ENABLED_ = false;
    static inline std::string OUT_DIR_ = "results2";
    static inline int WORKING_LIST_LIMIT_ = 8000;

    void createAll(int size);
    void finalCheck();

public:
    NetworkGenerator(int nbWires, int toSize);
    NetworkGenerator(int nbWires, int fromSize, int toSize, Network* prefix, Network* suffix);
    ~NetworkGenerator();

    std::vector<std::unique_ptr<Network>> createAll();
    Network* getPrefix() const;

    static bool isSubsumptionEnabled();
    static void setSubsumptionEnabled(bool enabled);
    static int getWorkingListLimit();
    static void setWorkingListLimit(int limit);
    static const std::string& getOutDir();
    static void setOutDir(const std::string& outDir);

    WorkingList* getWorkList() const { return workList_.get(); }
    std::shared_mutex& getWorkLock() { return workLock_; }
    std::mutex& getCleanupLock() { return cleanupLock_; }
    bool tryCleanupLock() { return cleanupLock_.try_lock(); }
    void unlockCleanup() { cleanupLock_.unlock(); }
    void incrementCheckedNetworks() { checkedNetworks_++; }

    long getTotalNetworks() const { return totalNetworks_; }
    long getCheckedNetworks() const { return checkedNetworks_; }
    void setRunningMonitor(bool running);
};
