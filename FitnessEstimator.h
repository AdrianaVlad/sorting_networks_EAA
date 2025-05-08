#pragma once

class Network;

class FitnessEstimator {
public:
    virtual double compute(const Network* net) const = 0;
    virtual ~FitnessEstimator() = default;
};
