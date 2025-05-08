#include "SortingNetworks.h"

const std::vector<int> SortingNetworks::OPT_SIZE = {
    0, 0, 1, 3, 5, 9, 12, 16, 19, 25, 29, 35, 39, 45, 51, 56, 60, 71
};

const std::vector<std::string> SortingNetworks::INSTANCES = {
    "", "", "", "", "", "",
    "[(0,1);(2,3);(0,4);(1,4);(2,5);(3,5);(1,3);(4,5);(0,2);(1,2);(2,4);(3,4)]",
    "[(0,1);(2,3);(4,5);(1,6);(3,6);(5,6);(0,2);(2,3);(1,4);(4,5);(3,5);(2,4);(3,4);(0,1);(1,3);(1,2)]",
    "[(0,1);(2,3);(4,5);(6,7);(1,3);(5,7);(0,2);(3,7);(4,6);(1,5);(3,5);(2,6);(5,6);(3,5);(0,4);(1,2);(2,4);(3,4);(1,2)]",
};

const std::vector<std::vector<double>> SortingNetworks::AVG_SIZE_PERCENT = {
    {}, {},
    {0.0, 100.0},
    {0.0, 77.67, 22.33},
    {0.0, 66.6, 26.6, 6.6},
    {0.0, 58.44, 26.0, 11.44, 4.11},
};

const std::vector<std::vector<double>> SortingNetworks::AVG_SIZE = {
    {0.0, 6.23, 3.37, 1.5, 0.63, 0.27},
    {0.0, 7.68, 4.38, 2.14, 1.07, 0.54, 0.19},
    {0.0, 8.71, 4.89, 2.73, 1.37, 0.77, 0.38, 0.14},
};

Network* SortingNetworks::getInstance(int nbWires) {
    if (nbWires >= static_cast<int>(INSTANCES.size())) return nullptr;
    auto* net = new Network(nbWires);
    net->parse(INSTANCES[nbWires]);
    return net;
}
