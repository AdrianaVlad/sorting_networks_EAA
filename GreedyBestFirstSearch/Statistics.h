#pragma once

#include <unordered_map>
#include <vector>
#include <string>

class Network;

class Statistics {
public:
    static inline const bool ENABLED = true;
    static inline const bool EXTENDED = true;
    static inline bool LOG = true;

    static inline int nbWires = 0;
    static inline int nbComparators = 0;
    static inline long long runningTime = 0;
    static inline long long usedMemory = 0;
    static inline long long finalChecksTime = 0;
    static inline int nbNetworks = 0;

    static inline long long subTotal = 0;
    static inline long long subDetected = 0;

    static inline int subOutputInclusion = 0;
    static inline int subClusterSizeFail = 0;
    static inline int subZeroOneSizeFail = 0;
    static inline int subZeroOnePermFail = 0;
    static inline int subValuesPermFail = 0;
    static inline int subPermutationFail = 0;
    static inline int redComparatorPos = 0;
    static inline int redSortedOutput = 0;

    static inline long long permTotal = 0;

    static inline std::unordered_map<Network*, Network*> subsumedMap;
    static inline std::unordered_map<Network*, Network*> failMap;
    static inline std::unordered_map<Network*, std::vector<int>> permMap;

    static void reset();
    static void print();
    static void print(std::ostream& out);
    static void log();
    static std::string getInfo();
    static std::string getExtendedInfo();
    static void logSubsumed(Network* net, Network* subsumedBy, const std::vector<int>& perm);
    static void logFail(Network* net0, Network* net1);
    static long long currentTimeMillis();
    static long long currentMemoryUsage();

private:
    static std::string formatTime(long long millis);
    static std::string padLeft(const std::string& s, int width);
};
