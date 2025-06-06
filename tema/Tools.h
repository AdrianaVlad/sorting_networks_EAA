#pragma once

#include <bitset>
#include <string>
#include <vector>
#include "Network.h"

namespace Tools {
    std::bitset<32> toBitSet(int value);
    int toInt(const std::bitset<32>& bits);
    std::string toBinaryString(int value, int nbits);

    void printGraph(const std::vector<std::vector<int>>& graph);
    void printCycle(const std::vector<int>& cycle);
    void printMatching(const std::vector<int>& matching);
    void printNetwork(const Network& net);
    void printNetworks(const Network& net0, const Network& net1);
    void printClusterSizes(const Network& net);

    void analyze(const Network& net);
    void analyze(const std::vector<Network*>& nets);
    void analyzeSorting(const std::vector<Network*>& nets);

    int countNonZeros(const std::vector<int>& vec);
    int combinations(int n, int k);
}
