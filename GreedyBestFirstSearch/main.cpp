#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <tuple>
#include <iomanip>
#include <windows.h>
#include <psapi.h>

#include "GreedyBestFirstSearch.h"
#include "FitnessArticleFormula.h"
#include "SubsumptionMatchImpl.h"
#include "GreenFilter.h"
#include "Statistics.h"

size_t getUsedMemoryInKB() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
}

void displayResultsInConsole(int n, int k, int bound, const std::vector<Network*>& results, const FitnessArticleFormula& fitness, double duration_sec) {
    std::cout << "Results for n=" << n << ", k=" << k << ", bound=" << bound << "\n";
    std::cout << "Execution time: " << std::fixed << std::setprecision(6) << duration_sec << " seconds\n";
    std::cout << "Total networks: " << results.size() << "\n";
    std::cout << "Memory used: " << getUsedMemoryInKB() << " KB\n\n";

    for (Network* net : results) {
        std::cout << "Network with " << net->size()
            << " comparators, fitness: "
            << fitness.compute(net) << "\n"
            << net->toString() << "\n";

        if (net->isSorting()) {
            std::cout << "Sorting network.\n";
        }
        else {
            std::cout << "NOT a sorting network.\n";
        }

        std::cout << "\n";
        delete net;
    }
}

void generateNetworksForParams(int n, int k, int bound) {
    FitnessArticleFormula fitness;
    SubsumptionMatchImpl subsumption;
    GreedyBestFirstSearch search(&fitness, &subsumption);
    GreenFilter* prefix = new GreenFilter(n);

    auto start = std::chrono::steady_clock::now();

    std::vector<Network*> results = search.generate(n, k, bound, prefix);

    auto end = std::chrono::steady_clock::now();
    auto duration_sec = std::chrono::duration<double>(end - start).count();

    displayResultsInConsole(n, k, bound, results, fitness, duration_sec);

    delete prefix;
}

int main() {
    std::vector<std::tuple<int, int, int>> param_combinations = {
        //{3, 3, 10},
        //{4, 5, 30},
        //{5, 9, 30},
        //{6, 12, 30},
        //{7, 16, 40},
        //{8, 19, 60},
        //{9, 25, 60},
        {10, 29, 70}
    };

    for (const auto& params : param_combinations) {
        int n, k, bound;
        std::tie(n, k, bound) = params;

        generateNetworksForParams(n, k, bound);
    }

    return 0;
}
