#include "FitnessComposite.h"
#include <stdexcept>

FitnessComposite::FitnessComposite(const std::vector<const FitnessEstimator*>& estimators,
    const std::vector<double>& weights)
    : estimators_(estimators), weights_(weights) {
    if (estimators_.size() != weights_.size()) {
        throw std::invalid_argument("Number of estimators must match number of weights.");
    }
}

double FitnessComposite::compute(const Network* net) const {
    double sum = 0.0;
    double totalWeight = 0.0;

    for (size_t i = 0; i < estimators_.size(); ++i) {
        double f = estimators_[i]->compute(net);
        sum += weights_[i] * f;
        totalWeight += weights_[i];
    }

    return totalWeight > 0.0 ? sum / totalWeight : 0.0;
}
