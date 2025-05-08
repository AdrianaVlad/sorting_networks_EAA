#pragma once

#include "Network.h"
#include "Statistics.h"
#include <vector>
#include <memory>
#include <string>

class NetworkIO {
public:
    static void ensureDirectoryExists(const std::string& path);
    static void write(int nbWires, int size, const std::vector<std::unique_ptr<Network>>& list);

    static void write(const std::string& dir, int nbWires, int size, const std::vector<std::unique_ptr<Network>>& list);

    static void writeOptimum(const std::string& dir, const Network& net);

    static void write(const std::string& dir, const std::string& file, const Network& net);

    static std::vector<std::unique_ptr<Network>> read(int nbWires, int size);

    static std::vector<std::unique_ptr<Network>> read(const std::string& dir, int nbWires, int nbComparators);

    static std::vector<std::unique_ptr<Network>> read(const std::string& dir, int nbWires, int nbComparators, int limit);

    static std::vector<std::unique_ptr<Network>> read(const std::string& dir, const std::string& file, int nbWires, int nbComparators, int limit);

    static std::unique_ptr<Network> readSingle(const std::string& dir, const std::string& file, int nbWires, int nbComparators);

    static void writeSubsumptions(int nbWires, int nbComparators);

    static void writeFails(int nbWires, int nbComparators);
};
