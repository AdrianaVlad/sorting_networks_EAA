#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Network.h"
#include "OutputCluster.h"
#include "Sequence.h"
#include "ValuesBitSet.h"

class OutputSet {
public:
    explicit OutputSet(Network* network);
    ~OutputSet();

    Network* getNetwork() const;

    OutputCluster* cluster(int level);
    const OutputCluster* cluster(int level) const;

    std::vector<OutputCluster*>& clusters();

    void add(const Sequence& sequence);
    void computeMinMaxValues();

    ValuesBitSet* bitValues() const;
    std::vector<int> intValues();

    bool contains(int value) const;
    int size() const;

    bool includes(const OutputSet& other) const;
    bool cannotSubsume(const OutputSet& other) const;

    int minClusterSize() const;
    int maxClusterSize() const;
    int minZeroCount() const;
    int maxZeroCount() const;
    int minOneCount() const;
    int maxOneCount() const;

    std::vector<int> posCount0();
    std::vector<int> posCount1();

    int getNbWires() const { return nbWires_; }

    std::vector<int> subsumes(const OutputSet& other);

    bool operator==(const OutputSet& other) const;
    std::string toString() const;
    std::string toStringZeros() const;
    std::string toStringOnes() const;
    std::string toStringIntValues() const;

private:
    void computePosCount();
    bool checkMatching(const OutputSet& other, const std::vector<int>& perm);
    std::vector<std::vector<int>> findMatching(const std::vector<std::vector<int>>& graph);
    bool matchRec(const std::vector<std::vector<int>>& graph, int u, int next,
        std::vector<bool>& seen, std::vector<std::vector<int>>& matching);
    std::vector<int> checkMatchings(const OutputSet& other,
        std::vector<std::vector<int>>& graph,
        std::vector<std::vector<int>>& degrees);
    std::vector<int> checkMatchingsRec(const OutputSet& other,
        std::vector<std::vector<int>>& graph,
        std::vector<std::vector<int>>& degrees,
        std::vector<std::vector<int>>& matching);
    std::vector<int> findCycle(const std::vector<std::vector<int>>& graph,
        const std::vector<std::vector<int>>& degrees,
        const std::vector<std::vector<int>>& matching);
    std::vector<int> findCycleRec(const std::vector<std::vector<int>>& graph,
        const std::vector<std::vector<int>>& degrees,
        const std::vector<std::vector<int>>& matching,
        int u, int pos, std::vector<std::vector<int>>& visited);

    Network* network_;
    std::vector<OutputCluster*> clusters_;
    std::unique_ptr<ValuesBitSet> values_;
    std::vector<int> intValues_;
    std::vector<int> posCount0_;
    std::vector<int> posCount1_;

    int nbWires_;
    int size_;

    int minClusterSize_;
    int maxClusterSize_;
    int minZeroCount_;
    int maxZeroCount_;
    int minOneCount_;
    int maxOneCount_;
};
