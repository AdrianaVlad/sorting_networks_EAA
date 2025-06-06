#include "NetworkExpander.h"
#include "Comparator.h"
#include "Sequence.h"
#include "ValuesBitSet.h"
#include "Statistics.h"

#include <mutex>
#include <random>
#include <cmath>
#include <shared_mutex>
#include <iostream>
#include <chrono>

static std::mutex coutMutex;

NetworkExpander::NetworkExpander(NetworkGenerator* generator, RuntimeNetwork* net)
    : generator_(generator), net_(net), workList_(generator->getWorkList()) {}

int NetworkExpander::operator()() {
    return expandAll();
}

int NetworkExpander::expandAll() {
    auto start = std::chrono::high_resolution_clock::now();

    int added = 0;
    int nbWires = net_->nbWires();

    for (int i = 0; i < nbWires - 1; ++i) {
        for (int j = i + 1; j < nbWires; ++j) {
            generator_->incrementCheckedNetworks();

            if (isRedundant(net_, i, j)) continue;

            auto net1 = std::make_unique<RuntimeNetwork>(net_, i, j);

            if (net_->computeFitness() == 1.0) {
                continue;
            }

            removeSubsumed(net1.get());
            workList_->addNetwork(std::move(net1));
            added++;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    /*
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[DEBUG] expandAll() took " << elapsed.count() << " seconds.\n";
    }
    */

    return added;
}

bool NetworkExpander::isSubsumed(RuntimeNetwork* net) {
    bool full = workList_->isFull();
    if (!NetworkGenerator::isSubsumptionEnabled() && !full) return false;

    net->checkedSubsumedById = workList_->getMaxId();
    double fitness = net->computeFitness();

    std::shared_lock lock(generator_->getWorkLock());

    for (int i = workList_->first(); i <= net->outSize; ++i) {
        NetworkList* list = workList_->networkList(i);
        for (int j = 0; j < list->size(); ++j) {
            RuntimeNetwork* other = list->getNetwork(j);
            if (other->isDead()) continue;

            if (NetworkGenerator::isSubsumptionEnabled() && other->subsumes(net)) {
                return true;
            }

            if (full && fitness > other->computeFitness()) {
                double r1 = static_cast<double>(rand()) / RAND_MAX;
                double r2 = static_cast<double>(rand()) / RAND_MAX;
                if (r1 < fitness && r2 > other->computeFitness()) {
                    return true;
                }
            }
        }
    }

    return false;
}

void NetworkExpander::removeSubsumed(RuntimeNetwork* net) {
    auto start = std::chrono::high_resolution_clock::now();

    net->checkedSubsumesId = workList_->getMaxId();
    int kills = 0;
    int killLimit = workList_->aliveSize() - NetworkGenerator::getWorkingListLimit();
    double fitness = net->computeFitness();

    std::shared_lock lock(generator_->getWorkLock());

    for (int i = net->outSize + 1; i <= workList_->last(); ++i) {
        NetworkList* list = workList_->networkList(i);
        for (int j = 0; j < list->size(); ++j) {
            RuntimeNetwork* other = list->getNetwork(j);
            if (other->isDead()) continue;

            if (NetworkGenerator::isSubsumptionEnabled() && net->subsumes(other)) {
                workList_->addDead(other);
                continue;
            }

            if (kills < killLimit && workList_->isFull()) {
                if (fitness < other->computeFitness()) {
                    double r1 = static_cast<double>(rand()) / RAND_MAX;
                    double r2 = static_cast<double>(rand()) / RAND_MAX;
                    if (r1 > fitness && r2 < other->computeFitness()) {
                        workList_->addDead(other);
                        kills++;
                    }
                }
            }
        }
    }

    if (workList_->deadSize() > 1000) {
        std::unique_lock wlock(generator_->getWorkLock());
        if (generator_->tryCleanupLock()) {
            workList_->removeAllDead();
            generator_->unlockCleanup();
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    /*
    std::chrono::duration<double> elapsed = end - start;
    {
        std::lock_guard<std::mutex> lock(coutMutex);
        std::cout << "[DEBUG] removeSubsumed() took " << elapsed.count() << " seconds.\n";
    }
    */
}

bool NetworkExpander::isRedundant(RuntimeNetwork* net, int wire0, int wire1) {
    Comparator* lastComp = net->lastComparator(wire0, wire1);
    if (lastComp != nullptr) {
        if (Statistics::ENABLED) Statistics::redComparatorPos++;
        return true;
    }

    bool redundant = true;
    OutputSet* out = net->outputSet();
    ValuesBitSet* values = out->bitValues();

    for (int value = values->nextSetBit(0); value >= 0; value = values->nextSetBit(value + 1)) {
        Sequence* seq = Sequence::getInstance(Statistics::nbWires, value);
        if (seq->get(wire0) && !seq->get(wire1)) {
            redundant = false;
            break;
        }
    }

    if (redundant && Statistics::ENABLED) {
        Statistics::redSortedOutput++;
    }

    return redundant;
}
