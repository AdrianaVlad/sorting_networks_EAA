#pragma once

#include "RuntimeNetwork.h"
#include <vector>
#include <memory>
#include <mutex>

class NetworkList {
private:
    std::vector<std::unique_ptr<RuntimeNetwork>> networks_;
    mutable std::mutex mtx_;
    int capacity_ = 10;

public:
    NetworkList();

    void clear();
    void addNetwork(std::unique_ptr<RuntimeNetwork> net);
    void remove(RuntimeNetwork* net);
    RuntimeNetwork* getNetwork(int i) const;
    int size() const;

    std::vector<std::unique_ptr<RuntimeNetwork>>& getNetworks();
    int getCapacity() const;

    void setCapacity(int capacity);
};
