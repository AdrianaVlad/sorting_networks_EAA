#pragma once

#include "Subsumption.h"

class SubsumptionVerifier {
public:
    static Subsumption* getInstance();

private:
    static Subsumption* instance_;
};
