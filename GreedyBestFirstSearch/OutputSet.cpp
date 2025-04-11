#include "OutputSet.h"
#include <sstream>
#include <limits>
#include <algorithm>

OutputSet::OutputSet(Network* network)
    : network_(network), nbWires_(network->nbWires()), values_(new ValuesBitSet()), size_(0),
    minClusterSize_(std::numeric_limits<int>::max()), maxClusterSize_(0),
    minZeroCount_(std::numeric_limits<int>::max()), maxZeroCount_(0),
    minOneCount_(std::numeric_limits<int>::max()), maxOneCount_(0) {

    clusters_.resize(nbWires_ + 1);
    for (int level = 0; level <= nbWires_; ++level) {
        clusters_[level] = new OutputCluster(this, level);
    }
    /*
    * std::cout << "[DEBUG] OutputSet constructed at " << this
        << ", nbWires = " << nbWires_
        << ", values_ = " << values_
        << ", clusters_ size = " << clusters_.size() << "\n";
    */

}

Network* OutputSet::getNetwork() const {
    return network_;
}

OutputCluster* OutputSet::cluster(int level) {
    return clusters_[level];
}

std::vector<OutputCluster*>& OutputSet::clusters() {
    return clusters_;
}

void OutputSet::add(const Sequence& sequence) {
    int level = sequence.cardinality();
    OutputCluster* cluster = clusters_[level];
    int value = cluster->add(sequence);
    if (value >= 0) {
        values_->set(value);
        ++size_;
    }
}

void OutputSet::computeMinMaxValues() {
    minClusterSize_ = std::numeric_limits<int>::max();
    maxClusterSize_ = 0;
    minZeroCount_ = std::numeric_limits<int>::max();
    maxZeroCount_ = 0;
    minOneCount_ = std::numeric_limits<int>::max();
    maxOneCount_ = 0;

    for (int k = 1; k < nbWires_; ++k) {
        OutputCluster* c = clusters_[k];
        int sz = c->size();
        minClusterSize_ = std::min(minClusterSize_, sz);
        maxClusterSize_ = std::max(maxClusterSize_, sz);

        int zc = c->zeroCount();
        minZeroCount_ = std::min(minZeroCount_, zc);
        maxZeroCount_ = std::max(maxZeroCount_, zc);

        int oc = c->oneCount();
        minOneCount_ = std::min(minOneCount_, oc);
        maxOneCount_ = std::max(maxOneCount_, oc);
    }
}

ValuesBitSet* OutputSet::bitValues() const {
    return values_.get();
}

std::vector<int> OutputSet::intValues() {
    if (!intValues_.empty()) return intValues_;
    for (int i = values_->nextSetBit(0); i >= 0; i = values_->nextSetBit(i + 1)) {
        intValues_.push_back(i);
    }
    return intValues_;
}

bool OutputSet::contains(int value) const {
    return values_->get(value);
}

int OutputSet::size() const {
    return size_;
}

bool OutputSet::includes(const OutputSet& other) const {
    if (other.size_ > size_) return false;

    ValuesBitSet* bs = other.bitValues();
    for (int value = bs->nextSetBit(0); value >= 0; value = bs->nextSetBit(value + 1)) {
        if (!values_->get(value)) {
            return false;
        }
    }
    return true;
}

bool OutputSet::cannotSubsume(const OutputSet& other) const {
    if (size_ > other.size_) return true;

    if (size_ < other.size_) {
        if (maxClusterSize_ > other.maxClusterSize_ ||
            minClusterSize_ > other.minClusterSize_ ||
            maxZeroCount_ > other.maxZeroCount_ ||
            minZeroCount_ > other.minZeroCount_ ||
            maxOneCount_ > other.maxOneCount_ ||
            minOneCount_ > other.minOneCount_) {
            return true;
        }
    }
    else if (maxClusterSize_ != other.maxClusterSize_ ||
        minClusterSize_ != other.minClusterSize_ ||
        maxZeroCount_ != other.maxZeroCount_ ||
        minZeroCount_ != other.minZeroCount_ ||
        maxOneCount_ != other.maxOneCount_ ||
        minOneCount_ != other.minOneCount_) {
        return true;
    }

    for (int k = 1; k < nbWires_; ++k) {
        if (clusters_[k]->cannotSubsume(*other.clusters_[k])) {
            return true;
        }
    }
    return false;
}

int OutputSet::minClusterSize() const { return minClusterSize_; }
int OutputSet::maxClusterSize() const { return maxClusterSize_; }
int OutputSet::minZeroCount() const { return minZeroCount_; }
int OutputSet::maxZeroCount() const { return maxZeroCount_; }
int OutputSet::minOneCount() const { return minOneCount_; }
int OutputSet::maxOneCount() const { return maxOneCount_; }

void OutputSet::computePosCount() {
    posCount0_.assign(nbWires_, 0);
    posCount1_.assign(nbWires_, 0);

    for (int i = values_->nextSetBit(0); i >= 0; i = values_->nextSetBit(i + 1)) {
        Sequence* s = Sequence::getInstance(nbWires_, i);
        int k = s->cardinality();
        for (int j = 0; j < nbWires_; ++j) {
            if (j < nbWires_ - k && s->get(j)) posCount1_[j]++;
            if (j >= nbWires_ - k && !s->get(j)) posCount0_[j]++;
        }
    }
}

std::vector<int> OutputSet::posCount0() {
    if (posCount0_.empty()) computePosCount();
    return posCount0_;
}

std::vector<int> OutputSet::posCount1() {
    if (posCount1_.empty()) computePosCount();
    return posCount1_;
}

bool OutputSet::operator==(const OutputSet& other) const {
    return *values_ == *(other.values_);
}

std::string OutputSet::toString() const {
    std::ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < clusters_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << clusters_[i]->toString();
    }
    oss << "}";
    return oss.str();
}

std::string OutputSet::toStringZeros() const {
    std::ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < clusters_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << clusters_[i]->toStringZeros();
    }
    oss << "}";
    return oss.str();
}

std::string OutputSet::toStringOnes() const {
    std::ostringstream oss;
    oss << "{";
    for (size_t i = 0; i < clusters_.size(); ++i) {
        if (i > 0) oss << ",";
        oss << clusters_[i]->toStringOnes();
    }
    oss << "}";
    return oss.str();
}

std::string OutputSet::toStringIntValues() const {
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (int i = values_->nextSetBit(0); i >= 0; i = values_->nextSetBit(i + 1)) {
        if (!first) oss << ",";
        oss << i;
        first = false;
    }
    oss << "}";
    return oss.str();
}

const OutputCluster* OutputSet::cluster(int level) const {
    return clusters_[level];
}

OutputSet::~OutputSet() {
    for (auto* cluster : clusters_) {
        delete cluster;
    }
}
