#pragma once

#include <vector>
#include <string>
#include "ValuesBitSet.h"
#include "Sequence.h"

class OutputSet;
class Network;

class OutputCluster {
public:
    OutputCluster(OutputSet* outputSet, int level);

    OutputSet* getOutputSet() const;
    Network* getNetwork() const;

    int add(const Sequence& sequence);

    int size() const;

    ValuesBitSet* bitValues();                      
    const ValuesBitSet* bitValues() const;          


    std::vector<int> intValues();

    std::vector<bool>& zeroPositions();
    std::vector<bool>& onePositions();

    int zeroCount() const;
    int oneCount() const;

    bool includes(const OutputCluster& other) const;
    bool cannotSubsume(const OutputCluster& other) const;

    bool operator==(const OutputCluster& other) const;

    std::string toString() const;
    std::string toStringZeros() const;
    std::string toStringOnes() const;

    const std::vector<bool>& getPos0() const { return pos0_; }
    const std::vector<bool>& getPos1() const { return pos1_; }


private:
    OutputSet* outputSet_;
    int level_;
    int nbWires_;
    int size_;

    ValuesBitSet* bitValues_;
    mutable std::vector<int> intValues_;
    int count0_;
    int count1_;
    std::vector<bool> pos0_;
    std::vector<bool> pos1_;

};
