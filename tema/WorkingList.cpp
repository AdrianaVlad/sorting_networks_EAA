#include "WorkingList.h"
#include "NetworkGenerator.h"
#include <iostream>
#include <limits>
#include <sstream>

WorkingList::WorkingList(int nbWires, int maxOutSize)
    : nbWires_(nbWires) {

    array_.reserve(maxOutSize);
    for (int i = 0; i < maxOutSize; ++i) {
        array_.emplace_back(std::make_unique<NetworkList>());
    }
    std::cout << "WorkingList initialized, length=" << maxOutSize << std::endl;
}

int WorkingList::size() const {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    return size_;
}

int WorkingList::aliveSize() const {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    return size_ - static_cast<int>(dead_.size());
}

void WorkingList::clear() {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    for (auto& list : array_) {
        list->clear();
    }
    size_ = 0;
    maxId_ = -1;
    first_ = std::numeric_limits<int>::max();
    last_ = -1;
    dead_.clear();
}

RuntimeNetwork* WorkingList::getNetwork(int outSize, int index) const {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    return array_[outSize]->getNetwork(index);
}

NetworkList* WorkingList::networkList(int outSize) const {
    return array_[outSize].get();
}

void WorkingList::addDead(RuntimeNetwork* net) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    if (net->dead) return;
    net->dead = true;
    dead_.push_back(net);
}

void WorkingList::removeAllDead() {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    for (auto* net : dead_) {
        array_[net->outSize]->remove(net);
    }
    size_ -= static_cast<int>(dead_.size());
    dead_.clear();

    while (first_ < static_cast<int>(array_.size()) && array_[first_]->size() == 0) {
        first_++;
    }
    while (last_ >= 0 && array_[last_]->size() == 0) {
        last_--;
    }
}

void WorkingList::addNetwork(std::unique_ptr<RuntimeNetwork> net) {
    std::lock_guard<std::recursive_mutex> lock(mtx_);
    net->createId();
    int outSize = net->outputSet()->size();

    if (outSize >= static_cast<int>(array_.size())) {
        throw std::out_of_range("OutSize exceeds array limit in WorkingList.");
    }

    int id = net->getId();

    array_[outSize]->addNetwork(std::move(net));

    if (maxId_ < id) {
        maxId_ = id;
    }
    if (outSize < first_) {
        first_ = outSize;
    }
    if (outSize > last_) {
        last_ = outSize;
    }
    size_++;
}

bool WorkingList::isFull() const {
    return aliveSize() >= NetworkGenerator::getWorkingListLimit();
}

std::vector<std::unique_ptr<RuntimeNetwork>>& WorkingList::networks(int outSize) {
    return array_[outSize]->getNetworks();
}

std::vector<std::unique_ptr<RuntimeNetwork>> WorkingList::joinLists() const {
    std::vector<std::unique_ptr<RuntimeNetwork>> all;
    for (int i = first_; i <= last_; ++i) {
        int n = array_[i]->size();
        for (int j = 0; j < n; ++j) {
            RuntimeNetwork* rawPtr = array_[i]->getNetwork(j);
            all.emplace_back(std::make_unique<RuntimeNetwork>(*rawPtr));
        }
    }
    return all;
}

std::string WorkingList::toString() const {
    std::ostringstream sb;
    for (int i = first_; i <= last_; ++i) {
        auto& list = array_[i];
        sb << "\nsize " << i << "\t= " << list->size();
    }
    return sb.str();
}
