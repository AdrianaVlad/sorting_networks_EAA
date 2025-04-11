#pragma once

#include <vector>

class ValuesBitSet {
public:
    ValuesBitSet();
    explicit ValuesBitSet(size_t size);

    void set(int index);
    bool get(int index) const;
    void clear(int index);
    void clear();

    int nextSetBit(int fromIndex) const;
    int cardinality() const;
    size_t size() const;
    bool isEmpty() const;

    void or_(const ValuesBitSet& other);
    void andNot(const ValuesBitSet& other);

    bool operator==(const ValuesBitSet& other) const;

private:
    std::vector<bool> bits_;
};
