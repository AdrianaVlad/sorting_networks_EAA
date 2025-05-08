#include "FitnessOutputSize.h"
#include <cmath>

double FitnessOutputSize::compute(const Network* net) const {
    double nbWires = static_cast<double>(net->nbWires());
    double outSize = static_cast<double>(net->outputSet()->size());
    return (outSize - (nbWires + 1)) / std::pow(2.0, nbWires);
}
