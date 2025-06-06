#include "Sequence.h"
#include <cmath>
#include <sstream>
#include <stdexcept>

std::vector<std::vector<Sequence*>> Sequence::CACHE;

void Sequence::initializeCache(int maxNbWires) {
    CACHE.resize(maxNbWires + 1);
    for (int k = 1; k <= maxNbWires; ++k) {
        int m = 1 << k;
        CACHE[k].resize(m, nullptr);
    }
}

Sequence::Sequence(int nbits)
    : nbits_(nbits), value_(0), cardinality_(0), bitSet_(nbits, false) {}

Sequence::Sequence(int nbits, int value)
    : nbits_(nbits), value_(value), cardinality_(0), bitSet_(nbits, false)
{
    if (nbits < static_cast<int>(CACHE.size()) &&
        value < static_cast<int>(CACHE[nbits].size()) &&
        CACHE[nbits][value]) {

        this->cardinality_ = CACHE[nbits][value]->cardinality_;
        this->bitSet_ = CACHE[nbits][value]->bitSet_;
    }
    else {
        setValue(value);
    }
}

Sequence::Sequence(const Sequence& other)
    : Sequence(other.nbits_, other.value_) {}

void Sequence::setValue(int value) {
    value_ = value;
    bitSet_.assign(nbits_, false);
    cardinality_ = 0;
    int temp = value;
    int index = 0;

    while (temp != 0) {
        bool bit = temp % 2 != 0;
        bitSet_[nbits_ - index - 1] = bit;
        if (bit) cardinality_++;
        temp >>= 1;
        index++;
    }
}

void Sequence::computeValue() {
    value_ = 0;
    for (int i = 0; i < nbits_; ++i) {
        if (bitSet_[nbits_ - i - 1]) {
            value_ += 1 << i;
        }
    }
}

int Sequence::length() const {
    return nbits_;
}

int Sequence::getValue() const {
    return value_;
}

int Sequence::cardinality() const {
    return cardinality_;
}

bool Sequence::get(int bitIndex) const {
    return bitSet_[bitIndex];
}

std::vector<bool> Sequence::getBitSet() const {
    return bitSet_;
}

Sequence* Sequence::getInstance(int nbits, int value) {
    if (nbits >= static_cast<int>(CACHE.size())) {
        return new Sequence(nbits, value);
    }

    if (CACHE[nbits][value] == nullptr) {
        CACHE[nbits][value] = new Sequence(nbits, value);
    }

    return CACHE[nbits][value];
}

Sequence* Sequence::getSwappedInstance(const Sequence* other, int idx0, int idx1) {
    int temp = 0;
    int n = other->nbits_;
    for (int i = 0; i < n; ++i) {
        int j = i;
        if (i == idx0) j = idx1;
        else if (i == idx1) j = idx0;

        if (other->bitSet_[j]) {
            temp += 1 << (n - i - 1);
        }
    }
    return getInstance(n, temp);
}

Sequence* Sequence::permute(const std::vector<int>& perm) const {
    int n = perm.size();
    int permValue = 0;
    for (int i = 0; i < n; ++i) {
        if (bitSet_[i]) {
            permValue += 1 << (nbits_ - perm[i] - 1);
        }
    }
    return getInstance(nbits_, permValue);
}

bool Sequence::isSorted() const {
    for (int j = 0; j < nbits_; ++j) {
        if ((j < nbits_ - cardinality_ && bitSet_[j]) ||
            (j >= nbits_ - cardinality_ && !bitSet_[j])) {
            return false;
        }
    }
    return true;
}

std::string Sequence::toString(int nbits) const {
    std::ostringstream oss;
    for (int i = 0; i < nbits; ++i) {
        oss << (bitSet_[i] ? "1" : "0");
    }
    return oss.str();
}

std::string Sequence::toString() const {
    return toString(nbits_);
}
