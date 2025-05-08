#pragma once

#include "Network.h"

class GreenFilter : public Network {
public:
    explicit GreenFilter(int nbWires);

private:
    void init();
};
