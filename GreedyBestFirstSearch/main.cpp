#include "NetworkGenerator.h"
#include "GreenFilter.h"
#include "FitnessBadPosCount.h"
#include "Statistics.h"
#include "Permutations.h"
#include "Sequence.h"
#include <iostream>
#include <memory>
#include <vector>

void generate(int nbWires, int fromSize, int toSize) {
    Permutations::get(0);
    Sequence::getInstance(nbWires, 0);

    GreenFilter prefix(nbWires);
    NetworkGenerator generator(nbWires, fromSize, toSize, &prefix, nullptr);

    auto networks = generator.createAll();
    for (const auto& net : networks) {
        std::cout << net->toString() << "\n";
    }
}


int main() {

    generate(7, 9, 16);

    return 0;
}