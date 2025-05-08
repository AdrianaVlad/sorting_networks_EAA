#include "RuntimeNetwork.h"
#include "SubsumptionVerifier.h"

using std::atomic;
using std::lock_guard;
using std::mutex;

atomic<int> RuntimeNetwork::maxId_{ 0 };
mutex RuntimeNetwork::idMutex_;

RuntimeNetwork::RuntimeNetwork(int nbWires)
    : Network(nbWires) {
    createId();
}

RuntimeNetwork::RuntimeNetwork(Network* net)
    : Network(*net) {
    createId();
}

RuntimeNetwork::RuntimeNetwork(Network* net, int i, int j)
    : Network(net, i, j) {
    outSize = outputSet()->size();
    outputSet()->computeMinMaxValues();
}

int RuntimeNetwork::getId() const {
    return id;
}

bool RuntimeNetwork::isDead() const {
    return dead;
}

void RuntimeNetwork::createId() {
    lock_guard<mutex> lock(idMutex_);
    id = maxId_++;
}

void RuntimeNetwork::resetIds() {
    maxId_ = 0;
}

bool RuntimeNetwork::subsumes(RuntimeNetwork* other) {
    if (other->dead) return false;
    std::vector<int> result = this->checkSubsumption(other);
    return !result.empty();
}
