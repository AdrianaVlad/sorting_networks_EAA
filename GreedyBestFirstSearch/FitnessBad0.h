#pragma once

#include "FitnessEstimator.h"

class FitnessBad0 : public FitnessEstimator {
public:
    double compute(const Network* net) const override;
};
