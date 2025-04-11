#include "SubsumptionVerifier.h"
#include "Config.h"
#include "SubsumptionMatchImpl.h"
#include <iostream>

Subsumption* SubsumptionVerifier::instance_ = nullptr;

Subsumption* SubsumptionVerifier::getInstance() {
    if (instance_ == nullptr) {
        std::string implName = Config::getSubsumptionImpl();

        if (implName == "SubsumptionMatchImpl") {
            instance_ = new SubsumptionMatchImpl();
        }
        else {
            std::cerr << "Unknown Subsumption implementation: " << implName << std::endl;
            return nullptr;
        }
    }
    return instance_;
}
