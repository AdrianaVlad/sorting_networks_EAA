#include "Statistics.h"
#include "Network.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <chrono>

void Statistics::reset() {
    finalChecksTime = 0;
    nbNetworks = 0;
    subTotal = 0;
    subDetected = 0;
    subOutputInclusion = 0;
    subClusterSizeFail = 0;
    subZeroOneSizeFail = 0;
    subZeroOnePermFail = 0;
    subValuesPermFail = 0;
    subPermutationFail = 0;
    redComparatorPos = 0;
    redSortedOutput = 0;
    permTotal = 0;
    subsumedMap.clear();
    failMap.clear();
    permMap.clear();
}

void Statistics::print() {
    std::cout << (EXTENDED ? getExtendedInfo() : getInfo()) << std::endl;
}

void Statistics::log() {
    if (!LOG) return;
    std::ofstream file("statistics.log", std::ios::app);
    if (file.is_open()) {
        file << getInfo() << std::endl;
        file.close();
    }
}

std::string Statistics::getInfo() {
    std::ostringstream oss;
    oss << padLeft("(" + std::to_string(nbWires) + "," + std::to_string(nbComparators) + ")", 8) << " | "
        << padLeft(std::to_string(nbNetworks), 10) << " | "
        << formatTime(runningTime) << " | "
        << padLeft(std::to_string(runningTime / 1000) + "s", 10) << " | ";

    auto now = std::chrono::system_clock::now();
    std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &t_c);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm);
    oss << buf;

    if (ENABLED) {
        oss << " | " << padLeft(std::to_string(subTotal), 15)
            << " | " << padLeft(std::to_string(subDetected), 10)
            << " | " << padLeft(std::to_string(permTotal), 12);
    }

    return oss.str();
}

std::string Statistics::getExtendedInfo() {
    std::ostringstream oss;
    auto now = std::chrono::system_clock::now();
    std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &t_c);
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm);

    oss << "(" << nbWires << "," << nbComparators << ") " << buf << "\n";
    oss << "Number of wires: " << nbWires << "\n";
    oss << "Number of comparators: " << nbComparators << "\n";
    oss << "Total networks identified: " << nbNetworks << "\n";
    oss << "Running time: " << formatTime(runningTime) << "\n";
    oss << "Final checks time: " << formatTime(finalChecksTime) << "\n";
    oss << "Used memory: " << usedMemory / (1024 * 1024) << " MB\n";

    if (ENABLED) {
        oss << "Total subsumptions checks: " << subTotal << "\n";
        oss << "\t- detected: " << subDetected
            << " (" << (subTotal > 0 ? 100.0 * subDetected / subTotal : 0) << "%)\n";
        oss << "\t- due to direct output inclusion: " << subOutputInclusion << "\n";
        oss << "\t- failed by cluster sizes: " << subClusterSizeFail << "\n";
        oss << "\t- failed by different zero/one sizes: " << subZeroOneSizeFail << "\n";
        oss << "\t- no permutation: " << subPermutationFail << "\n";
        oss << "\t- zero/one permutation fails: " << subZeroOnePermFail << "\n";
        oss << "\t- values permutation fails: " << subValuesPermFail << "\n";
        oss << "Redundancies\n";
        oss << "\t- due to comparator positions: " << redComparatorPos << "\n";
        oss << "\t- due to sorted output: " << redSortedOutput << "\n";
    }

    return oss.str();
}

std::string Statistics::formatTime(long long millis) {
    long long h = millis / (1000 * 60 * 60);
    millis %= 1000 * 60 * 60;
    long long m = millis / (1000 * 60);
    millis %= 1000 * 60;
    long long s = millis / 1000;
    millis %= 1000;

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << h << ":"
        << std::setw(2) << m << ":"
        << std::setw(2) << s << ":"
        << std::setw(3) << millis;
    return oss.str();
}

std::string Statistics::padLeft(const std::string& s, int width) {
    std::ostringstream oss;
    oss << std::setw(width) << std::right << s;
    return oss.str();
}

void Statistics::logSubsumed(Network* net, Network* subsumedBy, const std::vector<int>& perm) {
    subsumedMap[net] = subsumedBy;
    permMap[net] = perm;
}

void Statistics::logFail(Network* net0, Network* net1) {
    failMap[net0] = net1;
}
