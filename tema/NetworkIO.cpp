#include "NetworkIO.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

void NetworkIO::ensureDirectoryExists(const std::string& path) {
    if (!fs::exists(path)) {
        fs::create_directories(path);
    }
}

void NetworkIO::write(int nbWires, int size, const std::vector<std::unique_ptr<Network>>& list) {
    write("results", nbWires, size, list);
}

void NetworkIO::write(const std::string& dir, int nbWires, int size, const std::vector<std::unique_ptr<Network>>& list) {
    ensureDirectoryExists(dir);
    std::ofstream out(dir + "/networks_" + std::to_string(nbWires) + "-" + std::to_string(size) + ".txt");
    if (!out.is_open()) {
        std::cerr << "Cannot open file for writing networks.\n";
        return;
    }
    for (const auto& net : list) {
        out << net->toParseableString() << "\n";
        out << net->outputSet()->toStringIntValues() << "\n";
    }
}

void NetworkIO::writeOptimum(const std::string& dir, const Network& net) {
    write(dir, "optimum", net);
}

void NetworkIO::write(const std::string& dir, const std::string& file, const Network& net) {
    ensureDirectoryExists(dir);
    std::ofstream out(dir + "/" + file + "_" + std::to_string(net.nbWires()) + "-" + std::to_string(net.nbComparators()) + ".txt", std::ios::app);
    if (!out.is_open()) {
        std::cerr << "Cannot open file for writing single Network.\n";
        return;
    }
    out << net.toParseableString() << "\n";
    out << net.outputSet()->toStringIntValues() << "\n";
}

std::vector<std::unique_ptr<Network>> NetworkIO::read(int nbWires, int size) {
    return read("results", nbWires, size);
}

std::vector<std::unique_ptr<Network>> NetworkIO::read(const std::string& dir, int nbWires, int nbComparators) {
    return read(dir, nbWires, nbComparators, INT32_MAX);
}

std::vector<std::unique_ptr<Network>> NetworkIO::read(const std::string& dir, int nbWires, int nbComparators, int limit) {
    return read(dir, "networks", nbWires, nbComparators, limit);
}

std::vector<std::unique_ptr<Network>> NetworkIO::read(const std::string& dir, const std::string& file, int nbWires, int nbComparators, int limit) {
    std::vector<std::unique_ptr<Network>> list;
    std::string filename = dir + "/" + file + "_" + std::to_string(nbWires) + "-" + std::to_string(nbComparators) + ".txt";

    if (!fs::exists(filename)) {
        std::cout << "File " << filename << " does not exist..." << std::endl;
        return list;
    }

    std::ifstream in(filename);
    if (!in.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }

    std::cout << "Reading from " << filename << " ... ";

    std::string line;
    std::unique_ptr<Network> net = nullptr;
    while (std::getline(in, line)) {
        if (list.size() >= static_cast<size_t>(limit)) break;

        if (!line.empty() && line[0] == '[') {
            net = std::make_unique<Network>(nbWires);
            net->parse(line);
            list.push_back(std::move(net));
        }
        else if (!line.empty() && line[0] == '{' && !list.empty()) {
            list.back()->parseOutput(line);
            net = nullptr;
        }
    }

    std::cout << list.size() << " networks" << std::endl;
    return list;
}

std::unique_ptr<Network> NetworkIO::readSingle(const std::string& dir, const std::string& file, int nbWires, int nbComparators) {
    auto nets = read(dir, file, nbWires, nbComparators, 1);
    if (nets.empty()) return nullptr;
    return std::move(nets.front());
}

void NetworkIO::writeSubsumptions(int nbWires, int nbComparators) {
    ensureDirectoryExists("results");
    std::ofstream out("results/subsumptions_" + std::to_string(nbWires) + "-" + std::to_string(nbComparators) + ".txt");
    if (!out.is_open()) {
        std::cerr << "Cannot open file for writing subsumptions.\n";
        return;
    }

    for (const auto& [net1, net0] : Statistics::subsumedMap) {
        const auto* out0 = net0->outputSet();
        const auto* out1 = net1->outputSet();

        out << net0->toParseableString() << "\n";
        out << net1->toParseableString() << "\n";
        out << out0->toString() << "\n";
        out << out1->toString() << "\n";

        const auto& perm = Statistics::permMap.at(net1);
        for (int p : perm) {
            out << p << " ";
        }
        out << "\n";

        out << "0: " << out0->toStringZeros() << "\n";
        out << "0: " << out1->toStringZeros() << "\n";
        out << "1: " << out0->toStringOnes() << "\n";
        out << "1: " << out1->toStringOnes() << "\n";

        out << "\n";
    }
}

void NetworkIO::writeFails(int nbWires, int nbComparators) {
    ensureDirectoryExists("results");
    std::ofstream out("results/fails_" + std::to_string(nbWires) + "-" + std::to_string(nbComparators) + ".txt");
    if (!out.is_open()) {
        std::cerr << "Cannot open file for writing fails.\n";
        return;
    }

    for (const auto& [net0, net1] : Statistics::failMap) {
        const auto* out0 = net0->outputSet();
        const auto* out1 = net1->outputSet();

        out << net0->toParseableString() << "\n";
        out << net1->toParseableString() << "\n";
        out << out0->toString() << "\n";
        out << out1->toString() << "\n";

        out << "0: " << out0->toStringZeros() << "\n";
        out << "0: " << out1->toStringZeros() << "\n";

        for (int i = 0; i <= nbWires; ++i) {
            out << out0->cluster(i)->zeroCount() << ", ";
        }
        out << "\n";

        for (int i = 0; i <= nbWires; ++i) {
            out << out1->cluster(i)->zeroCount() << ", ";
        }
        out << "\n";

        out << "1: " << out0->toStringOnes() << "\n";
        out << "1: " << out1->toStringOnes() << "\n";

        for (int i = 0; i <= nbWires; ++i) {
            out << out0->cluster(i)->oneCount() << ", ";
        }
        out << "\n";

        for (int i = 0; i <= nbWires; ++i) {
            out << out1->cluster(i)->oneCount() << ", ";
        }
        out << "\n\n";
    }
}
