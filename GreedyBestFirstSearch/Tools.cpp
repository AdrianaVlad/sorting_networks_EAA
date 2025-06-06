#include "Tools.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>
#include <algorithm>

namespace Tools {

    std::vector<bool> Tools::toBitSet(int value, int bitsCount) {
        std::vector<bool> bitSet(bitsCount, false);
        for (int i = 0; i < bitsCount; ++i) {
            bitSet[i] = (value >> (bitsCount - i - 1)) & 1;
        }
        return bitSet;
    }

    int Tools::toInt(const std::vector<bool>& bitSet) {
        int value = 0;
        int n = static_cast<int>(bitSet.size());
        for (int i = 0; i < n; ++i) {
            if (bitSet[i]) {
                value |= (1 << (n - i - 1));
            }
        }
        return value;
    }


    std::string Tools::toBinaryString(const std::bitset<32>& bits, int nbits) {
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
