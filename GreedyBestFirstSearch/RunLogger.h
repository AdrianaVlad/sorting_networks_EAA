#pragma once

#include <string>
#include <vector>

class RunLogger {
public:
    static void logSingleRun(int nbWires, int comparators, double fitness, long long time, bool found, int nbNetworks, int runIndex);
    static void writeSummaryStats(int nbWires, int comparators, const std::vector<double>& fitnesses,
        const std::vector<long long>& times, int foundSortingCount,
        const std::vector<int>& networkCounts);
};
