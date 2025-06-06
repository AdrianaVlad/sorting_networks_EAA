#pragma once

#include <iostream>

class Comparator {
public:
    Comparator(int wire0, int wire1);

    void set(int wire0, int wire1);

    int getWire0() const;
    int getWire1() const;

    bool isAscending() const;

    int getDepth() const;
    void setDepth(int depth);

    bool operator==(const Comparator& other) const;
    bool operator<(const Comparator& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Comparator& c);

private:
    int wire0;
    int wire1;
    int depth;
};