#pragma once

#include "OutputSet.h"
#include "Subsumption.h"
#include "Statistics.h"
#include <vector>

class SubsumptionMatchImpl : public Subsumption {
public:
    SubsumptionMatchImpl();

    std::vector<int> findPermutation(const OutputSet& out0, const OutputSet& out1) override;

private:
    std::vector<int> checkMatchings(const OutputSet& out0, const OutputSet& out1,
        std::vector<std::vector<int>>& graph,
        std::vector<std::vector<int>>& degrees);

    std::vector<std::vector<int>> findMatching(const std::vector<std::vector<int>>& graph);

    bool matchRec(const std::vector<std::vector<int>>& graph, int u, int next,
        std::vector<bool>& seen, std::vector<std::vector<int>>& matching);

    std::vector<int> checkMatchingsRec(const OutputSet& out0, const OutputSet& out1,
        std::vector<std::vector<int>>& graph,
        std::vector<std::vector<int>>& degrees,
        std::vector<std::vector<int>>& matching);

    std::vector<int> findCycle(const std::vector<std::vector<int>>& graph,
        const std::vector<std::vector<int>>& degrees,
        const std::vector<std::vector<int>>& matching);

    std::vector<int> findCycleRec(const std::vector<std::vector<int>>& graph,
        const std::vector<std::vector<int>>& degrees,
        const std::vector<std::vector<int>>& matching,
        int u, int pos, std::vector<std::vector<int>>& visited);

    bool checkPermutation(const OutputSet& out0, const OutputSet& out1, const std::vector<int>& perm);
};
