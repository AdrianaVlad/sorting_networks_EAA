#include "Network.h"
#include "SortingNetworks.h"
#include "SubsumptionVerifier.h"
#include <cmath>
#include <random>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <regex>
#include <numeric>

Network::Network(int nbWires) : nbWires_(nbWires), last_(nbWires, -1), adjacents_(nbWires - 1, false) {
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
    OutputSet* originalOut = net->outputSet();
    auto values = originalOut->bitValues();

    addComparator(c);

    outputSet_ = new OutputSet(this);
    int wire0 = c.getWire0();
    int wire1 = c.getWire1();

    for (int value = values->nextSetBit(0); value >= 0; value = values->nextSetBit(value + 1)) {
        Sequence* seq = Sequence::getInstance(nbWires_, value);
        bool bit0 = seq->get(wire0);
        bool bit1 = seq->get(wire1);
        if (bit0 && !bit1) {
            seq = Sequence::getSwappedInstance(seq, wire0, wire1);
        }
        outputSet_->add(*seq);
    }

    prefix = net->prefix;
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
    int idx0 = last_[wire0];
    int idx1 = last_[wire1];
    if (idx0 >= 0 && idx0 == idx1) return true;

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

    int index = comparators_.size() - 1;
    last_[i] = index;
    last_[j] = index;


    if (std::abs(i - j) == 1) {
        if (i < nbWires_ - 1) {
            adjacents_[i] = true;
        }
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
    Comparator* lastComp = lastComparator(c->getWire0(), c->getWire1());
    if (!lastComp || !(*c == *lastComp)) {
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

Network* Network::untangle() {
    std::vector<Comparator> comps;
    for (const Comparator& c : comparators_) {
        comps.emplace_back(c.getWire0(), c.getWire1());
    }

    int nbComps = comps.size();

    for (int q = 0; q < nbComps; ++q) {
        Comparator& cq = comps[q];

        if (cq.isAscending()) continue;

        int iq = cq.getWire0();
        int jq = cq.getWire1();
        cq.set(jq, iq);

        for (int s = q + 1; s < nbComps; ++s) {
            Comparator& cs = comps[s];
            int is = cs.getWire0();
            int js = cs.getWire1();

            if (is == iq) is = jq;
            else if (is == jq) is = iq;

            if (js == iq) js = jq;
            else if (js == jq) js = iq;

            cs.set(is, js);
        }
    }

    Network* net = new Network(nbWires_);
    for (const Comparator& c : comps) {
        net->addComparator(c);
    }
    return net;
}

std::vector<int> Network::checkEquivalence(Network* other) {
    int n = nbWires_;
    std::vector<int> perm(n);
    std::iota(perm.begin(), perm.end(), 0);

    do {
        Network* pnet = this->permuteWires(perm);

        Network* untangled = pnet->untangle();

        if (untangled->toParseableString() == other->toParseableString()) {
            delete pnet;
            delete untangled;
            return perm;
        }

        delete pnet;
        delete untangled;

    } while (std::next_permutation(perm.begin(), perm.end()));

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
    int d0 = (last_[i] >= 0) ? comparators_[last_[i]].getDepth() : -1;
    int d1 = (last_[j] >= 0) ? comparators_[last_[j]].getDepth() : -1;
    return std::max(d0, d1) + 1;
}

std::string Network::toString() const {
    return toParseableString();
}

int Network::commonPrefix(Network* other) {
    int count = -1;
    int minSize = std::min(this->comparators_.size(), other->comparators_.size());
    for (int i = 0; i < minSize; ++i) {
        if (!(this->comparators_[i] == other->comparators_[i])) {
            break;
        }
        count++;
    }
    return count;
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
#include "FitnessArticleFormula.h"
#include "FitnessBad0.h"
#include "FitnessBad1.h"

//FitnessBadPosCount fitnessImpl;
//FitnessArticleFormula fitnessImpl;
FitnessBad0 fitnessImpl;
//FitnessBad1 fitnessImpl;
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
    int idx0 = last_[wire0];
    int idx1 = last_[wire1];
    if (idx0 == -1 || idx1 == -1) return nullptr;
    return (idx0 == idx1) ? const_cast<Comparator*>(&comparators_[idx0]) : nullptr;
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

Network* Network::createRandom(int nbWires, int size) {
    Network* net = new Network(nbWires);
    while (net->size() < size) {
        net->addRandomComparator();
    }
    return net;
}

Network* Network::permuteWires(const std::vector<int>& p) {
    if (p.size() != nbWires_) {
        throw std::invalid_argument("Permutation size must match number of wires: " + std::to_string(nbWires_));
    }

    Network* net = new Network(nbWires_);
    for (const Comparator& c : comparators_) {
        int i = c.getWire0();
        int j = c.getWire1();
        net->addComparator(p[i], p[j]);
    }

    return net;
}


Network* Network::split(int fromIndex) {
    if (fromIndex < 0 || fromIndex >= comparators_.size()) {
        throw std::invalid_argument("Split index must be between 0 and " + std::to_string(comparators_.size() - 1));
    }

    Network* net = new Network(nbWires_);
    for (size_t i = fromIndex; i < comparators_.size(); ++i) {
        const Comparator& c = comparators_[i];
        net->addComparator(c.getWire0(), c.getWire1());
    }
    return net;
}
