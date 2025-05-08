#include "Network.h"
#include "SortingNetworks.h"
#include "SubsumptionVerifier.h"
#include <cmath>
#include <random>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <regex>

Network::Network(int nbWires) : nbWires_(nbWires), last_(nbWires, nullptr), adjacents_(nbWires - 1, false) {
    generator = new OutputGenerator(this);
    //std::cout << "[DEBUG] Network(" << nbWires << ") constructor called at " << this << std::endl;
}

Network::Network(const Network& other) : Network(other.nbWires_) {
    //std::cout << "[DEBUG] Network copy constructor called at " << this << " from " << &other << std::endl;
    prefix = other.prefix;
    for (const auto& c : other.comparators_) {
        addComparator(c);
    }

    if (other.outputSet_ != nullptr) {
        outputSet_ = new OutputSet(this);
        auto values = other.outputSet_->bitValues();
        for (int v = values->nextSetBit(0); v >= 0; v = values->nextSetBit(v + 1)) {
            outputSet_->add(*Sequence::getInstance(nbWires_, v));
        }
    }
}

Network::Network(Network* net, int i, int j) : Network(*net) {
    //std::cout << "[DEBUG] Network(net, i, j) constructor called at " << this << std::endl;
    addComparator(i, j);
}

Network::Network(Network* net, const Comparator& c) : Network(*net) {
    //std::cout << "[DEBUG] Network(net, comparator) constructor called at " << this << std::endl;
    addComparator(c);
}

int Network::nbWires() const {
    return nbWires_;
}

int Network::size() const {
    return comparators_.size();
}

int Network::nbComparators() const {
    return size();
}

int Network::nbLayers() const {
    return layers_.size();
}

int Network::depth() const {
    return layers_.size();
}

bool Network::isEmpty() const {
    return comparators_.empty();
}

bool Network::isMaximal() const {
    return size() == depth() * (nbWires_ / 2);
}

bool Network::isGeneralized() const {
    return generalized;
}

double Network::computeFitness() const {
    if (fitness < 0 && fitnessEstimator) {
        fitness = fitnessEstimator->compute(this);
    }
    return fitness;
}

int Network::compareByFitness(const Network& other) {
    double f0 = computeFitness();
    double f1 = other.computeFitness();
    return (f0 < f1) ? -1 : (f0 > f1) ? 1 : 0;
}

bool Network::contains(int wire0, int wire1) {
    for (const auto& c : comparators_) {
        if (c.getWire0() == wire0 && c.getWire1() == wire1)
            return true;
    }
    return false;
}

bool Network::isRedundant(int wire0, int wire1) {
    Comparator* lastComp = last_[wire0];
    if (lastComp && last_[wire1] == lastComp)
        return true;

    OutputSet* out = outputSet();
    ValuesBitSet* values = out->bitValues();
    for (int value = values->nextSetBit(0); value >= 0; value = values->nextSetBit(value + 1)) {
        auto s = Sequence::getInstance(nbWires_, value);
        if (s->get(wire0) && !s->get(wire1)) {
            return false;
        }
    }

    return true;
}

void Network::addComparator(int i, int j) {
    addComparator(Comparator(i, j));
}

void Network::addComparator(const Comparator& c) {
    comparators_.push_back(c);
    int i = c.getWire0();
    int j = c.getWire1();

    if (i > j)
        generalized = true;

    int depth = computeDepth(i, j);

    Comparator& added = comparators_.back();
    added.setDepth(depth);

    if (depth < layers_.size()) {
        layers_[depth].add(added);
    }
    else {
        layers_.emplace_back(this, depth);
        layers_.back().add(added);
    }

    last_[i] = &added;
    last_[j] = &added;

    if (abs(i - j) == 1) {
        adjacents_[std::min(i, j)] = true;
    }

    outputSet_ = nullptr;
    fitness = -1;
}


void Network::addNetwork(const Network& net, const std::vector<int>& wires) {
    for (const auto& c : net.comparators_) {
        addComparator(wires[c.getWire0()], wires[c.getWire1()]);
    }
}

Comparator* Network::createRandomComparator() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 100);

    double p = dis(gen);
    int size = 1;
    double q = SortingNetworks::AVG_SIZE_PERCENT[nbWires_][1];
    while (size < nbWires_ - 1 && p > q) {
        size++;
        q += SortingNetworks::AVG_SIZE_PERCENT[nbWires_][size];
    }

    std::uniform_int_distribution<> wireDist(0, nbWires_ - size - 1);
    int wire0 = wireDist(gen);
    int wire1 = wire0 + size;

    return new Comparator(wire0, wire1);
}

Comparator* Network::addRandomComparator() {
    Comparator* c = createRandomComparator();
    if (!(*c == *(last_[c->getWire0()]))) {
        addComparator(*c);
        return c;
    }
    delete c;
    return nullptr;
}

OutputSet* Network::outputSet() const {
    if (!outputSet_) {
        outputSet_ = generator->createAll();
        outputSet_->computeMinMaxValues();
    }
    return outputSet_;
}

std::vector<int> Network::apply(const std::vector<int>& input) {
    return generator->apply(input);
}

int Network::apply(int input) {
    return generator->apply(input).getValue();
}

std::vector<Comparator>& Network::getComparators() {
    return comparators_;
}

std::vector<Layer>& Network::getLayers() {
    return layers_;
}

Layer& Network::layer(int index) {
    return layers_[index];
}

Layer& Network::lastLayer() {
    return layers_.back();
}

std::vector<int> Network::checkEquivalence(Network* other) {
    return {};
}

std::vector<int> Network::checkSubsumption(Network* other) {
    Subsumption* verifier = SubsumptionVerifier::getInstance();
    return verifier->check(this, other);
}

void Network::setPrefix(Network* p) {
    prefix = p;
}

Network* Network::getPrefix() {
    return prefix;
}

int Network::getPrefixSize() const {
    return prefix ? prefix->nbComparators() : 0;
}

int Network::computeDepth(int i, int j) {
    int d0 = last_[i] ? last_[i]->getDepth() : -1;
    int d1 = last_[j] ? last_[j]->getDepth() : -1;
    return std::max(d0, d1) + 1;
}

std::string Network::toString() const {
    return toParseableString();
}

std::string Network::toParseableString() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < comparators_.size(); ++i) {
        oss << comparators_[i];
        if (i < comparators_.size() - 1)
            oss << ";";
    }
    oss << "]";
    return oss.str();
}

#include "FitnessBadPosCount.h"

FitnessBadPosCount fitnessImpl;
FitnessEstimator* Network::fitnessEstimator = &fitnessImpl;


void Network::parse(const std::string& str) {
    int offset = 0;
    if (str.find("(0,") == std::string::npos) {
        offset = 1;
    }

    std::regex re("\\d+");
    std::sregex_iterator it(str.begin(), str.end(), re);
    std::sregex_iterator end;

    std::vector<int> tokens;
    while (it != end) {
        tokens.push_back(std::stoi(it->str()));
        ++it;
    }

    for (size_t i = 0; i + 1 < tokens.size(); i += 2) {
        int wire0 = tokens[i] - offset;
        int wire1 = tokens[i + 1] - offset;
        addComparator(wire0, wire1);
    }
}

bool Network::isSorting() const {
    return outputSet()->size() == nbWires_ + 1;
}


Network::~Network() {
    //std::cout << "[DEBUG] Network destructor called at " << this << std::endl;
}

Comparator* Network::lastComparator(int wire0, int wire1) const {
    if (wire0 >= last_.size() || wire1 >= last_.size()) return nullptr;
    if (last_[wire0] == nullptr || last_[wire1] == nullptr) return nullptr;
    return (last_[wire0] == last_[wire1]) ? last_[wire0] : nullptr;
}


void Network::parseOutput(const std::string& str) {
    if (!outputSet_) {
        outputSet_ = new OutputSet(this);
    }

    std::regex re("\\d+");
    std::sregex_iterator it(str.begin(), str.end(), re);
    std::sregex_iterator end;

    while (it != end) {
        int value = std::stoi(it->str());
        Sequence* seq = Sequence::getInstance(nbWires_, value);
        outputSet_->add(*seq);
        ++it;
    }
}

