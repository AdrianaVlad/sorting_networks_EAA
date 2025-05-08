#include "FitnessArticleFormula.h"
#include "Network.h"
#include "OutputSet.h"
#include <cmath>

double FitnessArticleFormula::compute(const Network* net) const {
    int n = net->nbWires();
    int factor = static_cast<int>(std::pow(2, n));
    int normalizer = (n + 1) * (factor - 1);

    OutputSet* out = net->outputSet();
    std::vector<int> pos0 = out->posCount0();
    std::vector<int> pos1 = out->posCount1();

    int bad0 = 0, bad1 = 0;
    for (int i : pos0)
        if (i != 0) ++bad0;
    for (int i : pos1)
        if (i != 0) ++bad1;

    int badTotal = bad0 + bad1;
    int outputSize = out->size();

    double weightedScore = factor * badTotal + (outputSize - n - 1);
    return static_cast<double>(weightedScore) / normalizer;
}
