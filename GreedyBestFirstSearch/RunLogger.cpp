#include "RunLogger.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <numeric>
#include <cmath>
#include <iomanip>

namespace fs = std::filesystem;

void RunLogger::logSingleRun(int nbWires, int comparators, double fitness, long long time, bool found, int nbNetworks, int runIndex) {
    fs::create_directories("results");
    std::string filename = "results/runs_stats_" + std::to_string(nbWires) + "-" + std::to_string(comparators) + "_run" + std::to_string(runIndex) + ".csv";
    std::ofstream out(filename, std::ios::trunc);
    if (!out.is_open()) return;
    out << "run,fitness,time(ms),foundSorting,nbNetworks\n";
    out << runIndex << "," << fitness << "," << time << "," << (found ? 1 : 0) << "," << nbNetworks << "\n";
}

void RunLogger::writeSummaryStats(int nbWires, int comparators, const std::vector<double>& fitnesses,
    const std::vector<long long>& times, int foundSortingCount,
    const std::vector<int>& networkCounts) {
    fs::create_directories("results");
    std::string file = "results/summary_stats_" + std::to_string(nbWires) + "-" + std::to_string(comparators) + ".txt";
    std::ofstream out(file, std::ios::trunc);
    if (!out.is_open()) return;

    auto avg = [](const auto& v) {
        return std::accumulate(v.begin(), v.end(), 0.0) / v.size();
        };

    auto stdev = [](const auto& v, double mean) {
        double sum = 0;
        for (double val : v)
            sum += (val - mean) * (val - mean);
        return std::sqrt(sum / v.size());
        };

    double avgFitness = avg(fitnesses);
    double avgTime = avg(times);
    double avgCount = avg(networkCounts);

    double stdFitness = stdev(fitnesses, avgFitness);
    double stdTime = stdev(times, avgTime);

    out << std::fixed << std::setprecision(6);
    out << "Summary for (" << nbWires << ", " << comparators << ")\n";
    out << "-----------------------------------------\n";
    out << "Found sorting networks: " << foundSortingCount << " / " << fitnesses.size() << "\n\n";
    out << "Fitness (avg): " << avgFitness << "\n";
    out << "Fitness (stdev): " << stdFitness << "\n\n";
    out << "Time (avg ms): " << avgTime << "\n";
    out << "Time (stdev ms): " << stdTime << "\n\n";
    out << "Network count (avg): " << avgCount << "\n";
}
