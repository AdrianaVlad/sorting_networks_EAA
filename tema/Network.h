#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Comparator.h"
#include "Layer.h"
#include "OutputGenerator.h"
#include "OutputSet.h"
#include "Sequence.h"
#include "FitnessEstimator.h"

class OutputGenerator;

class Network {
public:
    Network(int nbWires);
    Network(const Network& other);
    Network(Network* net, int i, int j);
    Network(Network* net, const Comparator& c);

    ~Network();

    int nbWires() const;
    int size() const;
    int nbComparators() const;
    int nbLayers() const;
    int depth() const;
    bool isEmpty() const;
    bool isMaximal() const;
    bool isGeneralized() const;
    double computeFitness() const;
    int compareByFitness(const Network& other);
    bool contains(int wire0, int wire1);
    bool isRedundant(int wire0, int wire1);

    void addComparator(int i, int j);
    void addComparator(const Comparator& c);
    void addNetwork(const Network& net, const std::vector<int>& wires);
    Comparator* createRandomComparator();
    Comparator* addRandomComparator();

    OutputSet* outputSet() const;
    std::vector<int> apply(const std::vector<int>& input);
    int apply(int input);

    const std::vector<Comparator>& comparators() const;
    std::vector<Layer>& getLayers();
    Layer& layer(int index);
    Layer& lastLayer();

    std::vector<int> checkEquivalence(Network* other);
    std::vector<int> checkSubsumption(Network* other);

    void parse(const std::string& str);
    void parseOutput(const std::string& str);
    std::string toParseableString() const;
    std::string toString() const;
    int commonPrefix(Network* other);
    Network* split(int fromIndex);

    static Network* createRandom(int nbWires, int size);
    static int compareByClusters(Network* net0, Network* net1);

    Network* permuteWires(const std::vector<int>& perm);
    Network* permuteComparators(const std::vector<int>& perm);
    Network* untangle();

    void setPrefix(Network* prefix);
    Network* getPrefix();
    int getPrefixSize() const;

    std::vector<Comparator>& getComparators();
    bool isSorting() const;
    Comparator* lastComparator(int wire0, int wire1) const;

protected:
    int nbWires_;
    bool generalized = false;
    mutable double fitness = -1.0;

    std::vector<Comparator> comparators_;
    std::vector<Layer> layers_;
    std::vector<Comparator*> last_;
    std::vector<bool> adjacents_;

    mutable OutputSet* outputSet_ = nullptr;
    OutputGenerator* generator = nullptr;
    Network* prefix = nullptr;

public:
    static FitnessEstimator* fitnessEstimator;

private:
    int computeDepth(int i, int j);
};
