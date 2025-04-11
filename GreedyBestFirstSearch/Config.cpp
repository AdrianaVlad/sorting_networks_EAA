#include "Config.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>

std::unordered_map<std::string, std::string> Config::props;
bool Config::initialized = false;

void Config::init(const std::string& filename) {
    if (!initialized) {
        load(filename);
        initialized = true;
    }
}

void Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not load config file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        auto pos = line.find('=');
        if (pos != std::string::npos && line[0] != '#') {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            props[key] = value;
        }
    }
}

std::string Config::getSubsumptionImpl() {
    return props.count("subsumption") ? props["subsumption"] : "SubsumptionDefaultImpl";
}

bool Config::isTracingEnabled() {
    return props.count("tracing") && props["tracing"] == "true";
}

int Config::getMaxNbWires() {
    return props.count("maxWires") ? std::stoi(props["maxWires"]) : 10;
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
