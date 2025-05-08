#pragma once

#include "Subsumption.h"
#include <vector>
#include <functional>

class SubsumptionBipartiteMatching : public Subsumption {
public:
    std::vector<int> findPermutation(const OutputSet& out0, const OutputSet& out1) override;

private:
    int n;
    std::vector<std::vector<bool>> graph;

    void buildSubsumptionGraph(const OutputSet& out0, const OutputSet& out1);
    bool findInitialMatching(std::vector<int>& matchTo);
    void enumerateMatchings(std::vector<int>& matchTo, std::function<bool(const std::vector<int>&)> callback);
    void dfsEnumerate(std::vector<int>& match, std::vector<bool>& used, int u,
        std::function<bool(const std::vector<int>&)> callback);
};
