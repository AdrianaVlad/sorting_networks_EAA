#pragma once

#include <vector>
#include "Network.h"
#include "OutputSet.h"
#include "OutputCluster.h"
#include "ValuesBitSet.h"

class Subsumption {
public:
    virtual ~Subsumption() = default;

    virtual std::vector<int> findPermutation(const OutputSet& out0, const OutputSet& out1) = 0;

    std::vector<int> check(Network* net0, Network* net1);

protected:
    bool cannotSubsume(const OutputCluster& c0, const OutputCluster& c1) const;

    bool checkPermutation(const OutputCluster& c0, const OutputCluster& c1, const std::vector<int>& perm) const;
    bool checkPermutation(const OutputSet& out0, const OutputSet& out1, const std::vector<int>& perm) const;
};
