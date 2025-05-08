#pragma once

#include <string>
#include <vector>
#include "Network.h"

class SortingNetworks {
public:
    static Network* getInstance(int nbWires);

    static const std::vector<int> OPT_SIZE;
    static const std::vector<std::string> INSTANCES;
    static const std::vector<std::vector<double>> AVG_SIZE_PERCENT;
    static const std::vector<std::vector<double>> AVG_SIZE;

private:
    SortingNetworks() = default;
};
