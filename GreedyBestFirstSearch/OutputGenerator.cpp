#include "OutputGenerator.h"
#include <cmath>
#include <random>
#include <stdexcept>
#include <algorithm>

class Network;

OutputGenerator::OutputGenerator(Network* network)
    : network_(network),
    nbWires_(network->nbWires()),
    maxInputSize_(static_cast<int>(std::pow(2, nbWires_))) {}

Sequence OutputGenerator::apply(const Sequence& input) const {
    Sequence output = input;
    const auto& comps = network_->getComparators();

    for (const auto& c : comps) {
        bool ascending = c.isAscending();
        int i = c.getWire0();
        int j = c.getWire1();
        bool bit0 = output.get(i);
        bool bit1 = output.get(j);
        if ((ascending && bit0 && !bit1) || (!ascending && !bit0 && bit1)) {
            output = *Sequence::getSwappedInstance(&output, i, j);
        }
    }

    return output;
}

Sequence OutputGenerator::apply(int input) const {
    return apply(*Sequence::getInstance(nbWires_, input));
}

std::vector<int> OutputGenerator::apply(const std::vector<int>& input) const {
    int n = network_->nbWires();
    if (input.size() != static_cast<size_t>(n)) {
        throw std::invalid_argument("The number of input values must equal the number of wires.");
    }

    std::vector<int> output = input;
    const auto& comps = network_->getComparators();

    for (const auto& c : comps) {
        int i = c.getWire0();
        int j = c.getWire1();
        if ((c.isAscending() && output[i] > output[j]) || (!c.isAscending() && output[i] < output[j])) {
            std::swap(output[i], output[j]);
        }
    }

    return output;
}

OutputSet* OutputGenerator::createAll() const {
    auto* outputSet = new OutputSet(network_);
    for (int value = 0; value < maxInputSize_; ++value) {
        Sequence input = *Sequence::getInstance(nbWires_, value);
        Sequence output = apply(input);
        outputSet->add(output);
    }
    return outputSet;
}

OutputSet* OutputGenerator::createSampled(int sampleSize) const {
    auto* sampledOutput = new OutputSet(network_);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.0, 1.0);

    double p = static_cast<double>(sampleSize) / maxInputSize_;
    for (int value = 0; value < maxInputSize_; ++value) {
        if (dist(gen) > p) {
            continue;
        }
        Sequence input = *Sequence::getInstance(nbWires_, value);
        Sequence output = apply(input);
        sampledOutput->add(output);
    }

    return sampledOutput;
}
