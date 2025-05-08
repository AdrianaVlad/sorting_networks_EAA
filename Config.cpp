#include "Config.h"
#include <thread>

std::unordered_map<std::string, std::string> Config::props;
bool Config::initialized = false;

void Config::init() {
    if (!initialized) {
        props["subsumption"] = "SubsumptionMatchImpl";
        props["tracing"] = "true";
        props["maxWires"] = "18";
        props["threads"] = "4";
        props["monitorTime"] = "1000";

        initialized = true;
    }
}

std::string Config::getSubsumptionImpl() {
    return props.count("subsumption") ? props["subsumption"] : "SubsumptionMatchImpl";
}

bool Config::isTracingEnabled() {
    return props.count("tracing") && props["tracing"] == "true";
}

int Config::getMaxNbWires() {
    return props.count("maxWires") ? std::stoi(props["maxWires"]) : 18;
}

int Config::getNbThreads() {
    if (props.count("threads")) {
        int val = std::stoi(props["threads"]);
        return val == 0 ? std::thread::hardware_concurrency() : val;
    }
    return 8;
}

int Config::getMonitorTime() {
    return props.count("monitorTime") ? std::stoi(props["monitorTime"]) : 0;
}
