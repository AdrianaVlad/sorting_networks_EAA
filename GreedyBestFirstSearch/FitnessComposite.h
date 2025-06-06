#pragma once

#include "FitnessEstimator.h"
#include <vector>

class FitnessComposite : public FitnessEstimator {
public:
    FitnessComposite(const std::vector<const FitnessEstimator*>& estimators,
        const std::vector<double>& weights);

    double compute(const Network* net) const override;

private:
    std::vector<const FitnessEstimator*> estimators_;
    std::vector<double> weights_;
};
