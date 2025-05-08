#include "OutputCluster.h"
#include "OutputSet.h"
#include "Network.h"
#include "Tools.h"

#include <stdexcept>
#include <sstream>
#include <bitset>
#include <cstring>

OutputCluster::OutputCluster(OutputSet* outputSet, int level)
    : outputSet_(outputSet), level_(level), bitValues_(new ValuesBitSet()), size_(0),
    count0_(0), count1_(0), pos0_(nbWires_, false), pos1_(nbWires_, false){
    nbWires_ = outputSet->getNetwork()->nbWires();
    pos0_.resize(nbWires_, false);
    pos1_.resize(nbWires_, false);
}

OutputSet* OutputCluster::getOutputSet() const {
    return outputSet_;
}

Network* OutputCluster::getNetwork() const {
    return outputSet_->getNetwork();
}

int OutputCluster::add(const Sequence& sequence) {
    int k = sequence.cardinality();
    if (k != level_) {
        throw std::invalid_argument("Number of ones differs from cluster level.");
    }

    int value = sequence.getValue();
    if (bitValues_->get(value)) {
        return -1;
    }

    bitValues_->set(value);
    const std::vector<bool>& bits = sequence.getBitSet();

    for (int i = 0; i < nbWires_; ++i) {
        if (!pos0_[i] && !bits[i]) {
            pos0_[i] = true;
            count0_++;
        }
        if (!pos1_[i] && bits[i]) {
            pos1_[i] = true;
            count1_++;
        }
    }

    size_++;
    return value;
}


int OutputCluster::size() const {
    return size_;
}

ValuesBitSet* OutputCluster::bitValues() {
    return bitValues_;
}

std::vector<int> OutputCluster::intValues() {
    if (!intValues_.empty()) return intValues_;
    for (int i = bitValues_->nextSetBit(0); i >= 0; i = bitValues_->nextSetBit(i + 1)) {
        intValues_.push_back(i);
    }
    return intValues_;
}

std::vector<bool>& OutputCluster::zeroPositions() {
    return pos0_;
}

std::vector<bool>& OutputCluster::onePositions() {
    return pos1_;
}

int OutputCluster::zeroCount() const {
    return count0_;
}

int OutputCluster::oneCount() const {
    return count1_;
}

bool OutputCluster::includes(const OutputCluster& other) const {
    if (other.size_ > this->size_) return false;
    ValuesBitSet* v0 = this->bitValues_;
    ValuesBitSet* v1 = other.bitValues_;

    ValuesBitSet temp(v1->size());
    temp.or_(*v1);
    temp.andNot(*v0);
    return temp.isEmpty();
}

bool OutputCluster::cannotSubsume(const OutputCluster& other) const {
    return size_ > other.size_ || count0_ > other.count0_ || count1_ > other.count1_;
}

bool OutputCluster::operator==(const OutputCluster& other) const {
    return level_ == other.level_ && *bitValues_ == *(other.bitValues_);
}

std::string OutputCluster::toString() const {
    std::ostringstream oss;
    oss << "{";
    for (int i = bitValues_->nextSetBit(0); i >= 0; i = bitValues_->nextSetBit(i + 1)) {
        oss << Tools::toBinaryString(i, nbWires_) << ",";
    }
    oss << "}";
    return oss.str();
}

std::string OutputCluster::toStringZeros() const {
    std::string s;
    for (int i = 0; i < nbWires_; ++i) {
        s += pos0_[i] ? "0" : "_";
    }
    return s;
}

std::string OutputCluster::toStringOnes() const {
    std::string s;
    for (int i = 0; i < nbWires_; ++i) {
        s += pos1_[i] ? "1" : "_";
    }
    return s;
}

const ValuesBitSet* OutputCluster::bitValues() const {
    return bitValues_;
}
