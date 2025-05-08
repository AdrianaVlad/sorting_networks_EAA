#pragma once

#include "NetworkList.h"
#include "RuntimeNetwork.h"
#include <vector>
#include <memory>
#include <mutex>

class WorkingList {
private:
    std::vector<std::unique_ptr<NetworkList>> array_;
    std::vector<RuntimeNetwork*> dead_;
    mutable std::recursive_mutex mtx_;

    int nbWires_;
    int size_ = 0;
    int maxId_ = -1;
    int first_ = std::numeric_limits<int>::max();
    int last_ = -1;

public:
    WorkingList(int nbWires, int maxOutSize);

    int size() const;
    int aliveSize() const;
    void clear();

    RuntimeNetwork* getNetwork(int outSize, int index) const;
    NetworkList* networkList(int outSize) const;

    void addDead(RuntimeNetwork* net);
    void removeAllDead();
    void addNetwork(std::unique_ptr<RuntimeNetwork> net);

    bool isFull() const;
    std::vector<std::unique_ptr<RuntimeNetwork>> joinLists() const;

    int first() const { return first_; }
    int last() const { return last_; }
    int deadSize() const { return static_cast<int>(dead_.size()); }
    int getMaxId() const { return maxId_; }

    std::vector<std::unique_ptr<RuntimeNetwork>>& networks(int outSize);

    std::string toString() const;

    std::recursive_mutex& getMutex() { return mtx_; }
    const std::recursive_mutex& getMutex() const { return mtx_; }
};
