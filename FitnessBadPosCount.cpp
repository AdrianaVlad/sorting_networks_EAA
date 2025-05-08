#include "FitnessBadPosCount.h"
#include "Network.h"
#include "OutputSet.h"
#include "OutputCluster.h"
#include "Comparator.h"
#include <cmath>

double FitnessBadPosCount::compute(const Network* net) const {
    int n = net->nbWires();
    int factor = static_cast<int>(std::pow(2, n));

    OutputSet* out = net->outputSet();
    std::vector<int> pos0 = out->posCount0();
    std::vector<int> pos1 = out->posCount1();

    double bad0 = 0, bad1 = 0;
    for (int i : pos0) {
        if (i != 0) bad0 += 1;
    }
    for (int i : pos1) {
        if (i != 0) bad1 += 1;
    }

    double score = factor * (bad0 + bad1) + out->size();
    double fitness = (score - (n + 1)) / (factor * 2 * (n - 1) + factor);
    return fitness;
}
