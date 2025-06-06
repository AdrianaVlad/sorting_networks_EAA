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

    int badTotal = 0;
    for (int i = 0; i < n; ++i)
        if (pos0[i] != 0 || pos1[i] != 0)
            ++badTotal;

    int outputSize = out->size();

    double weightedScore = factor * badTotal + (outputSize - n - 1);
    return static_cast<double>(weightedScore) / normalizer;
}
