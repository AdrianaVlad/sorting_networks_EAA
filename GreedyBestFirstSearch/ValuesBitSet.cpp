#include "ValuesBitSet.h"
#include <stdexcept>
#include <algorithm>

ValuesBitSet::ValuesBitSet() = default;

ValuesBitSet::ValuesBitSet(size_t size) : bits_(size, false) {}

void ValuesBitSet::set(int index) {
    if (index < 0) {
        throw std::out_of_range("Index cannot be negative.");
    }
    if (static_cast<size_t>(index) >= bits_.size()) {
        bits_.resize(index + 1, false);
    }
    bits_[index] = true;
}

bool ValuesBitSet::get(int index) const {
    if (index < 0 || static_cast<size_t>(index) >= bits_.size()) {
        return false;
    }
    return bits_[index];
}

void ValuesBitSet::clear(int index) {
    if (index < 0 || static_cast<size_t>(index) >= bits_.size()) {
        return;
    }
    bits_[index] = false;
}

void ValuesBitSet::clear() {
    std::fill(bits_.begin(), bits_.end(), false);
}

int ValuesBitSet::nextSetBit(int fromIndex) const {
    if (fromIndex < 0) {
        throw std::out_of_range("fromIndex cannot be negative.");
    }
    for (size_t i = fromIndex; i < bits_.size(); ++i) {
        if (bits_[i]) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int ValuesBitSet::cardinality() const {
    return static_cast<int>(std::count(bits_.begin(), bits_.end(), true));
}

size_t ValuesBitSet::size() const {
    return bits_.size();
}

bool ValuesBitSet::isEmpty() const {
    return std::none_of(bits_.begin(), bits_.end(), [](bool b) { return b; });
}

void ValuesBitSet::or_(const ValuesBitSet& other) {
    size_t maxSize = std::max(bits_.size(), other.bits_.size());
    bits_.resize(maxSize, false);
    for (size_t i = 0; i < other.bits_.size(); ++i) {
        bits_[i] = bits_[i] || other.bits_[i];
    }
}

void ValuesBitSet::andNot(const ValuesBitSet& other) {
    size_t minSize = std::min(bits_.size(), other.bits_.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (other.bits_[i]) {
            bits_[i] = false;
        }
    }
}

bool ValuesBitSet::operator==(const ValuesBitSet& other) const {
    return bits_ == other.bits_;
}
