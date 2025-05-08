#pragma once

#include "FitnessEstimator.h"
#include <vector>

class Network;

class FitnessArticleFormula : public FitnessEstimator {
public:
    double compute(const Network* net) const override;
};
