#pragma once

#include <vector>
#include <string>

class Sequence {
private:
    int nbits_;
    int value_;
    int cardinality_;
    std::vector<bool> bitSet_;

    static std::vector<std::vector<Sequence*>> CACHE;

    Sequence(int nbits);
    Sequence(int nbits, int value);
    void setValue(int value);
    void computeValue();

public:
    Sequence(const Sequence& other);

    static void initializeCache(int maxNbWires);
    static Sequence* getInstance(int nbits, int value);
    static Sequence* getSwappedInstance(const Sequence* other, int idx0, int idx1);

    int length() const;
    int getValue() const;
    int cardinality() const;
    bool get(int bitIndex) const;
    std::vector<bool> getBitSet() const;
    Sequence* permute(const std::vector<int>& perm) const;
    bool isSorted() const;

    std::string toString(int nbits) const;
    std::string toString() const;
};
