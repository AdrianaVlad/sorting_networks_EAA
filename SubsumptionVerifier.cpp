#include "SubsumptionVerifier.h"
#include "Config.h"
#include "SubsumptionMatchImpl.h"
#include "SubsumptionBipartiteMatching.h"
#include <iostream>
#include <map>
#include <functional>

Subsumption* SubsumptionVerifier::instance_ = nullptr;

Subsumption* SubsumptionVerifier::getInstance() {
    if (instance_ == nullptr) {
        std::string implName = Config::getSubsumptionImpl();

        static const std::map<std::string, std::function<Subsumption* ()>> factory = {
            {"SubsumptionMatchImpl", []() { return new SubsumptionMatchImpl(); }},
            {"SubsumptionBipartiteMatching", []() { return new SubsumptionBipartiteMatching(); }}
        };

        auto it = factory.find(implName);
        if (it != factory.end()) {
            instance_ = it->second();
        }
        else {
            std::cerr << "Unknown implementation: " << implName << "\n";
            instance_ = nullptr;
        }
    }
    return instance_;
}
