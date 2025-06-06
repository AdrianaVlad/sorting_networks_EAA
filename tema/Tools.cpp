#include "Tools.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>

namespace Tools {

    std::bitset<32> toBitSet(int value) {
        return std::bitset<32>(value);
    }

    int toInt(const std::bitset<32>& bits) {
        return static_cast<int>(bits.to_ulong());
    }

    std::string toBinaryString(int value, int nbits) {
        std::bitset<32> bits(value);
        std::ostringstream oss;
        for (int i = nbits - 1; i >= 0; --i) {
            oss << bits[i];
        }
        return oss.str();
    }

    void printNetwork(const Network& net) {
        std::cout << "-------------------------------------------------\n";
        std::cout << net.toString() << std::endl;

        OutputSet* out = net.outputSet();
        std::cout << out->toString() << std::endl;

        std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        for (int i = 0; i <= net.nbWires(); ++i) {
            std::cout << out->cluster(i)->size() << ", ";
        }
        std::cout << "= " << out->size() << "\n";

        std::cout << "Fitness: " << net.computeFitness() << std::endl;
    }

    int countNonZeros(const std::vector<int>& vec) {
        return std::count_if(vec.begin(), vec.end(), [](int v) { return v != 0; });
    }

    int combinations(int n, int k) {
        double result = 1.0;
        for (int i = 0; i < k; ++i) {
            result *= (n - i) / double(i + 1);
        }
        return static_cast<int>(result);
    }

}
