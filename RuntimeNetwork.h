#pragma once

#include "Network.h"
#include <atomic>
#include <mutex>

class RuntimeNetwork : public Network {
private:
    static std::atomic<int> maxId_;
    static std::mutex idMutex_;

public:
    int id = -1;
    int checkedSubsumedById = -1;
    int checkedSubsumesId = -1;
    bool dead = false;
    int outSize = 0;

    explicit RuntimeNetwork(int nbWires);
    explicit RuntimeNetwork(Network* net);
    RuntimeNetwork(Network* net, int i, int j);
    int getId() const;
    bool isDead() const;
    void createId();
    static void resetIds();

    bool subsumes(RuntimeNetwork* other);
};
