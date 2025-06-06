#include "NetworkGenerator.h"
#include "NetworkExpander.h"
#include "NetworkRemover.h"
#include <iostream>
#include <cmath>
#include <fstream>


NetworkGenerator::NetworkGenerator(int nbWires, int toSize)
    : nbWires_(nbWires), fromSize_(toSize), toSize_(toSize),
    prefix_(nullptr), suffix_(nullptr) {

    list_.reserve(1000);
    int maxOutSize = static_cast<int>(std::pow(2, nbWires_));
    list_.emplace_back(std::make_unique<RuntimeNetwork>(nbWires_));

    Statistics::nbWires = nbWires_;
    workList_ = std::make_unique<WorkingList>(nbWires_, maxOutSize);
    threadPool_ = std::make_unique<FastThreadPool>(Config::getNbThreads());
}

NetworkGenerator::NetworkGenerator(int nbWires, int fromSize, int toSize, Network* prefix, Network* suffix)
    : nbWires_(nbWires), fromSize_(fromSize), toSize_(toSize),
    prefix_(prefix), suffix_(suffix) {

    list_.reserve(1000);
    int maxOutSize = 0;

    if (fromSize_ == 1) {
        list_.emplace_back(std::make_unique<RuntimeNetwork>(nbWires_));
        maxOutSize = static_cast<int>(std::pow(2, nbWires_));
    }
    else if (!prefix_) {
        auto nets = NetworkIO::read(OUT_DIR_, nbWires_, fromSize_ - 1);
        for (auto& net : nets) {
            list_.emplace_back(std::make_unique<RuntimeNetwork>(net.get()));
            maxOutSize = std::max(maxOutSize, net->outputSet()->size());
        }
    }
    else {
        std::cout << "Using prefix: " << prefix_->toString() << std::endl;
        list_.emplace_back(std::make_unique<RuntimeNetwork>(prefix_));
        maxOutSize = prefix_->outputSet()->size();
    }

    Statistics::nbWires = nbWires_;
    workList_ = std::make_unique<WorkingList>(nbWires_, maxOutSize);
    threadPool_ = std::make_unique<FastThreadPool>(Config::getNbThreads());
}

NetworkGenerator::~NetworkGenerator() = default;

std::vector<std::unique_ptr<Network>> NetworkGenerator::createAll() {
    for (int size = fromSize_; size <= toSize_; ++size) {
        createAll(size);
    }

    if (monitor_) {
        monitor_->setRunning(false);
    }

    std::vector<std::unique_ptr<Network>> result;
    result.reserve(list_.size());
    for (auto& net : list_) {
        result.emplace_back(std::make_unique<Network>(*net));
    }
    return result;
}

#include <chrono>

void NetworkGenerator::createAll(int size) {
    using clock = std::chrono::high_resolution_clock;
    auto totalStart = clock::now();

    long t0 = Statistics::currentTimeMillis();
    Statistics::reset();
    Statistics::nbComparators = size;
    workList_->clear();
    RuntimeNetwork::resetIds();

    totalNetworks_ = static_cast<long>(list_.size()) * nbWires_ * (nbWires_ - 1) / 2;
    checkedNetworks_ = 0;

    try {
        //std::cout << "[DEBUG] Submitting tasks to ThreadPool...\n";

        auto submitStart = clock::now();
        for (auto& net : list_) {
            if (net->isEmpty()) {
                workList_->addNetwork(std::make_unique<RuntimeNetwork>(net.get(), 0, 1));
                ++checkedNetworks_;
                continue;
            }
            threadPool_->submit(NetworkExpander(this, net.get()));
        }
        auto submitEnd = clock::now();
        /*
        std::cout << "[DEBUG] Submitting done. Took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(submitEnd - submitStart).count()
            << " ms.\n";
        */

        monitor_ = std::make_unique<MonitorThread>(this);
        monitor_->start();

        auto waitStart = clock::now();
        threadPool_->wait();
        auto waitEnd = clock::now();
        /*
        std::cout << "[DEBUG] ThreadPool wait() done. Took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(waitEnd - waitStart).count()
            << " ms.\n";
        */

        if (monitor_) {
            monitor_->setRunning(false);
        }

        auto finalCheckStart = clock::now();
        finalCheck();
        auto finalCheckEnd = clock::now();
        /*
        std::cout << "[DEBUG] Final check done. Took "
            << std::chrono::duration_cast<std::chrono::milliseconds>(finalCheckEnd - finalCheckStart).count()
            << " ms.\n";
        */

        list_ = workList_->joinLists();
    }
    catch (const std::exception& e) {
        std::cerr << "Exception during createAll: " << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (list_.size() > static_cast<size_t>(WORKING_LIST_LIMIT_)) {
        std::cout << "Trimming from " << list_.size() << std::endl;
        list_.resize(WORKING_LIST_LIMIT_);
    }

    long t1 = Statistics::currentTimeMillis();
    Statistics::runningTime = t1 - t0;
    Statistics::usedMemory = Statistics::currentMemoryUsage();
    Statistics::nbNetworks = static_cast<int>(list_.size());

    bool foundSorting = false;
    double bestFitness = 1.0;
    for (const auto& net : list_) {
        double fitness = net->computeFitness();
        bestFitness = std::min(bestFitness, fitness);
        if (net->isSorting()) foundSorting = true;
    }

    std::string baseName = "statistics_" + std::to_string(nbWires_) + "-" + std::to_string(size);
    std::string statsFile = "results/" + baseName + "_run" + std::to_string(runIndex_) + ".txt";

    std::ofstream out(statsFile);
    Statistics::print();
    if (out.is_open()) {
        Statistics::print(out);

        for (const auto& net : list_) {
            out << "Network with " << net->size()
                << " comparators, fitness: "
                << net->computeFitness() << "\n"
                << net->toString() << "\n";

            if (net->isSorting()) {
                out << "Sorting network.\n";
            }
            else {
                out << "NOT a sorting network.\n";
            }

            out << "\n";
        }

        out.close();
    }
    else {
        std::cerr << "Could not open file " << statsFile << " for writing statistics.\n";
    }
    /*
    if (Statistics::ENABLED) {
        NetworkIO::writeSubsumptions(nbWires_, size);
        NetworkIO::writeFails(nbWires_, size);
    }
    else {
        Statistics::log();
    }

    for (const auto& net : list_) {
        if (net->isSorting()) {
            std::cout << "\tFound!" << std::endl;
        }
    }

    */

    auto totalEnd = clock::now();
    /*
    std::cout << "[DEBUG] TOTAL createAll() took "
        << std::chrono::duration_cast<std::chrono::milliseconds>(totalEnd - totalStart).count()
        << " ms.\n";
    */
}


void NetworkGenerator::finalCheck() {
    long long start = Statistics::currentTimeMillis();

    workList_->removeAllDead();

    for (int i = workList_->first(); i <= workList_->last(); ++i) {
        auto& networks = workList_->networks(i);
        for (auto& net : networks) {
            threadPool_->submit(NetworkRemover(this, net.get()));
        }
    }

    threadPool_->wait();

    workList_->removeAllDead();


    Statistics::finalChecksTime += Statistics::currentTimeMillis() - start;

}

Network* NetworkGenerator::getPrefix() const {
    return prefix_;
}

bool NetworkGenerator::isSubsumptionEnabled() {
    return SUBSUMPTION_ENABLED_;
}

void NetworkGenerator::setSubsumptionEnabled(bool enabled) {
    SUBSUMPTION_ENABLED_ = enabled;
}

int NetworkGenerator::getWorkingListLimit() {
    return WORKING_LIST_LIMIT_;
}

void NetworkGenerator::setWorkingListLimit(int limit) {
    WORKING_LIST_LIMIT_ = limit;
}

const std::string& NetworkGenerator::getOutDir() {
    return OUT_DIR_;
}

void NetworkGenerator::setOutDir(const std::string& outDir) {
    OUT_DIR_ = outDir;
}

void NetworkGenerator::setRunningMonitor(bool running) {
    if (monitor_) {
        monitor_->setRunning(running);
    }
}
