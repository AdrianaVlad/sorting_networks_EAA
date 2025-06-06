#include "FitnessBad1.h"
#include "Network.h"
#include "OutputSet.h"
#include <cmath>

double FitnessBad1::compute(const Network* net) const {
    int n = net->nbWires();
    int factor = 1 << n;

    OutputSet* out = net->outputSet();
    std::vector<int> pos1 = out->posCount1();

    int bad1 = 0;
    for (int val : pos1) {
        if (val != 0) ++bad1;
    }

    int outputSize = out->size();
    int normalizer = (n + 1) * (factor - 1);
    double weightedScore = factor * bad1 + (outputSize - n - 1);

    return static_cast<double>(weightedScore) / normalizer;
}
