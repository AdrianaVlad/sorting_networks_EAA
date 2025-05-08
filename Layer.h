#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Comparator.h"

class Network;

class Layer {
public:
    Layer(Network* network, int level);

    Network* getNetwork() const;
    int getLevel() const;

    void add(const Comparator& comp);

    const std::vector<Comparator>& getComparators() const;
    int size() const;

    bool operator==(const Layer& other) const;
    bool operator<(const Layer& other) const;

    std::string toString() const;

private:
    Network* network;
    int level;
    std::vector<Comparator> comparators;
};
