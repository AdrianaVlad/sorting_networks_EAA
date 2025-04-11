#pragma once

#include "FitnessEstimator.h"
#include <vector>

class Network;

class FitnessBadPosCount : public FitnessEstimator {
public:
    double compute(const Network* net) const override;
};
