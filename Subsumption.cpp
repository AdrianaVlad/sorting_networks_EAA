#include "Subsumption.h"
#include "Permutations.h"
#include "Sequence.h"
#include "Statistics.h"

std::vector<int> Subsumption::check(Network* net0, Network* net1) {
    if (Statistics::ENABLED) {
        Statistics::subTotal++;
    }

    OutputSet* out0 = net0->outputSet();
    OutputSet* out1 = net1->outputSet();

    if (out0->cannotSubsume(*out1)) {
        if (Statistics::ENABLED) {
            Statistics::subClusterSizeFail++;
        }
        return {};
    }

    if (out1->includes(*out0)) {
        if (Statistics::ENABLED) {
            Statistics::subOutputInclusion++;
        }
        return Permutations::identity(net0->nbWires());
    }

    std::vector<int> perm = findPermutation(*out0, *out1);
    if (Statistics::ENABLED && !perm.empty()) {
        Statistics::subDetected++;
    }

    return perm;
}

bool Subsumption::cannotSubsume(const OutputCluster& c0, const OutputCluster& c1) const {
    return c0.size() > c1.size() ||
        c0.zeroCount() > c1.zeroCount() ||
        c0.oneCount() > c1.oneCount();
}

bool Subsumption::checkPermutation(const OutputCluster& c0, const OutputCluster& c1, const std::vector<int>& perm) const {
    const ValuesBitSet* values0 = c0.bitValues();
    const ValuesBitSet* values1 = c1.bitValues();
    int n = c0.getNetwork()->nbWires();

    for (int value0 = values0->nextSetBit(0); value0 >= 0; value0 = values0->nextSetBit(value0 + 1)) {
        int value1 = Sequence::getInstance(n, value0)->permute(perm)->getValue();
        if (!values1->get(value1)) {
            return false;
        }
    }
    return true;
}

bool Subsumption::checkPermutation(const OutputSet& out0, const OutputSet& out1, const std::vector<int>& perm) const {
    for (int k = 2; k < out0.getNbWires() - 1; ++k) {
        if (!checkPermutation(*out0.cluster(k), *out1.cluster(k), perm)) {
            return false;
        }
    }
    return true;
}
