#include "FitnessBad0.h"
#include "Network.h"
#include "OutputSet.h"
#include <cmath>

double FitnessBad0::compute(const Network* net) const {
    int n = net->nbWires();
    int factor = 1 << n;

    OutputSet* out = net->outputSet();
    std::vector<int> pos0 = out->posCount0();

    int bad0 = 0;
    for (int val : pos0) {
        if (val != 0) ++bad0;
    }

    int outputSize = out->size();
    int normalizer = (n + 1) * (factor - 1);
    double weightedScore = factor * bad0 + (outputSize - n - 1);

    return static_cast<double>(weightedScore) / normalizer;
}
