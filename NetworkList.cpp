#include "NetworkList.h"

NetworkList::NetworkList() {
    networks_.reserve(capacity_);
}

void NetworkList::clear() {
    std::lock_guard<std::mutex> lock(mtx_);
    networks_.clear();
}

void NetworkList::addNetwork(std::unique_ptr<RuntimeNetwork> net) {
    std::lock_guard<std::mutex> lock(mtx_);
    networks_.push_back(std::move(net));
}

void NetworkList::remove(RuntimeNetwork* net) {
    std::lock_guard<std::mutex> lock(mtx_);
    networks_.erase(
        std::remove_if(networks_.begin(), networks_.end(),
            [net](const std::unique_ptr<RuntimeNetwork>& ptr) { return ptr.get() == net; }),
        networks_.end()
    );
}

RuntimeNetwork* NetworkList::getNetwork(int i) const {
    std::lock_guard<std::mutex> lock(mtx_);
    return networks_[i].get();
}

int NetworkList::size() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return static_cast<int>(networks_.size());
}

std::vector<std::unique_ptr<RuntimeNetwork>>& NetworkList::getNetworks() {
    return networks_;
}

int NetworkList::getCapacity() const {
    return capacity_;
}

void NetworkList::setCapacity(int capacity) {
    capacity_ = capacity;
}
