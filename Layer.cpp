#include "Layer.h"
#include <algorithm>
#include <sstream>

Layer::Layer(Network* network, int level)
    : network(network), level(level) {}

Network* Layer::getNetwork() const {
    return network;
}

int Layer::getLevel() const {
    return level;
}

void Layer::add(const Comparator& c) {
    comparators.push_back(c);
}

const std::vector<Comparator>& Layer::getComparators() const {
    return comparators;
}

int Layer::size() const {
    return static_cast<int>(comparators.size());
}

bool Layer::operator==(const Layer& other) const {
    if (this->size() != other.size()) {
        return false;
    }

    for (const auto& c : comparators) {
        if (std::find(other.comparators.begin(), other.comparators.end(), c) == other.comparators.end()) {
            return false;
        }
    }

    return true;
}

bool Layer::operator<(const Layer& other) const {
    if (this->size() != other.size()) {
        return this->size() < other.size();
    }
    if (*this == other) {
        return false;
    }

    auto list0 = comparators;
    auto list1 = other.comparators;
    std::sort(list0.begin(), list0.end());
    std::sort(list1.begin(), list1.end());

    for (size_t i = 0; i < list0.size(); ++i) {
        if (list0[i] < list1[i]) return true;
        if (list1[i] < list0[i]) return false;
    }
    return false;
}

std::string Layer::toString() const {
    std::ostringstream oss;
    oss << "L" << level << ":[";
    for (size_t i = 0; i < comparators.size(); ++i) {
        oss << comparators[i];
        if (i != comparators.size() - 1) oss << "; ";
    }
    oss << "]";
    return oss.str();
}
