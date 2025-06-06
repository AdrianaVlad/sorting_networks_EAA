#pragma once

#include "FitnessEstimator.h"

class FitnessBad1 : public FitnessEstimator {
public:
    double compute(const Network* net) const override;
};
