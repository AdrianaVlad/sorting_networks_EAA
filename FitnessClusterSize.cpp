#include "FitnessClusterSize.h"
#include <cmath>

double FitnessClusterSize::compute(const Network* net) const {
    double score = 0.0;
    int n = net->nbWires();
    const OutputSet* out = net->outputSet();

    for (int i = 1; i < n - 1; ++i) {
        const OutputCluster* cluster = out->cluster(i);
        int k = std::abs(i - n / 2);
        score += std::pow(2.0, k) * (cluster->size() - 1);
    }

    return score == 0.0 ? 0.0 : 1.0 - 1.0 / score;
}
