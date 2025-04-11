#pragma once

#include <string>
#include <unordered_map>

class Config {
public:
    static void init(const std::string& filename = "config.properties");

    static std::string getSubsumptionImpl();
    static bool isTracingEnabled();
    static int getMaxNbWires();
    static int getNbThreads();
    static int getMonitorTime();

private:
    static std::unordered_map<std::string, std::string> props;
    static bool initialized;

    static void load(const std::string& filename);
};
