#include "Comparator.h"

Comparator::Comparator(int wire0, int wire1) : wire0(wire0), wire1(wire1), depth(0) {}

void Comparator::set(int wire0, int wire1) {
    this->wire0 = wire0;
    this->wire1 = wire1;
}

int Comparator::getWire0() const {
    return wire0;
}

int Comparator::getWire1() const {
    return wire1;
}

bool Comparator::isAscending() const {
    return wire0 < wire1;
}

int Comparator::getDepth() const {
    return depth;
}

void Comparator::setDepth(int d) {
    depth = d;
}

bool Comparator::operator==(const Comparator& other) const {
    return wire0 == other.wire0 && wire1 == other.wire1;
}

bool Comparator::operator<(const Comparator& other) const {
    if (depth != other.depth) return depth < other.depth;
    if (wire0 != other.wire0) return wire0 < other.wire0;
    return wire1 < other.wire1;
}

std::ostream& operator<<(std::ostream& os, const Comparator& c) {
    os << "(" << c.wire0 << "," << c.wire1 << ")";
    return os;
}
