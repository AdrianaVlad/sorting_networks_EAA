#include "Config.h"
#include "NetworkGenerator.h"
#include "GreenFilter.h"
#include <iostream>
#include <vector>
#include <tuple>
#include "FitnessBadPosCount.h"

int main() {
    std::vector<std::tuple<int, int, int>> inputConfigs = {
        {3, 3, 3},
        {4, 5, 5},
        {5, 9, 9},
        {6, 12, 12},
        {7, 16, 16},
        {8, 19, 19},
        {9, 25, 25},
        {10, 29, 29},
        {11, 35, 35},
        {12, 39, 39},
        {13, 45, 45},
        {14, 51, 51},
        {15, 56, 56},
        {16, 60, 60}
    };


    for (const auto& [nbWires, fromSize, toSize] : inputConfigs) {
        std::cout << "Testing config: nbWires=" << nbWires
            << ", fromSize=" << fromSize
            << ", toSize=" << toSize << std::endl;

        GreenFilter prefix(nbWires);
        NetworkGenerator generator(nbWires, fromSize, toSize, &prefix, nullptr);
        auto networks = generator.createAll();

        for (const auto& net : networks) {
            std::cout << "Network with " << net->size()
                << " comparators, fitness: "
                << net->computeFitness() << "\n"
                << net->toString() << "\n";

            if (net->isSorting()) {
                std::cout << "Sorting network.\n";
            }
            else {
                std::cout << "NOT a sorting network.\n";
            }

            std::cout << "\n";
        }
    }

    return 0;
}
