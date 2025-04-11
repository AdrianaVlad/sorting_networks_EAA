#pragma once

#include <vector>
#include "Network.h"
#include "FitnessEstimator.h"
#include "Subsumption.h"

class GreedyBestFirstSearch {
public:
    GreedyBestFirstSearch(FitnessEstimator* estimator, Subsumption* subsumption);
    std::vector<Network*> generate(int n, int k, int bound, Network* prefix);

private:
    FitnessEstimator* fitnessEstimator_;
    Subsumption* subsumption_;
};
