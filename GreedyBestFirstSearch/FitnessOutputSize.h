#pragma once
#include "FitnessEstimator.h"
#include "Network.h"

class FitnessOutputSize : public FitnessEstimator {
public:
    double compute(const Network* net) const override;
};
