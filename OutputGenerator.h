#pragma once

#include <vector>
#include "Network.h"
#include "Sequence.h"
#include "OutputSet.h"

class OutputSet;
class Network;


class OutputGenerator {
public:
    explicit OutputGenerator(Network* network);

    Sequence apply(const Sequence& input) const;
    Sequence apply(int input) const;
    std::vector<int> apply(const std::vector<int>& input) const;

    OutputSet* createAll() const;
    OutputSet* createSampled(int sampleSize) const;

private:
    Network* network_;
    int nbWires_;
    int maxInputSize_;
};
