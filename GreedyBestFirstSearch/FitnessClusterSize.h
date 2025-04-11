#pragma once
#include "FitnessEstimator.h"
#include "Network.h"

class FitnessClusterSize : public FitnessEstimator {
public:
    double compute(const Network* net) const override;
};
