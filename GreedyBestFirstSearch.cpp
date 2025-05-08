#include "GreedyBestFirstSearch.h"
#include <iostream>
#include <random>
#include <algorithm>

GreedyBestFirstSearch::GreedyBestFirstSearch(FitnessEstimator* estimator, Subsumption* subsumption)
    : fitnessEstimator_(estimator), subsumption_(subsumption) {}

std::vector<Network*> GreedyBestFirstSearch::generate(int n, int k, int bound, Network* prefix) {
    std::vector<Network*> Rp_prev, Rp;
    Rp_prev.push_back(new Network(*prefix));

    for (int p = prefix->nbComparators() + 1; p <= k; ++p) {
        Rp.clear();

        for (Network* C : Rp_prev) {
            for (int i = 0; i < n - 1; ++i) {
                for (int j = i + 1; j < n; ++j) {
                    if (C->isRedundant(i, j)) continue;

                    Network* C_star = new Network(C, i, j);

                    std::vector<Network*> new_Rp;
                    for (Network* Cprim : Rp) {
                        if (!subsumption_->check(C_star, Cprim).empty()) {
                            delete Cprim;
                            continue;
                        }

                        if (Rp.size() >= bound &&
                            fitnessEstimator_->compute(C_star) < fitnessEstimator_->compute(Cprim)) {
                            std::random_device rd;
                            std::mt19937 gen(rd());
                            std::uniform_real_distribution<> dis(0, 1);
                            double x = dis(gen);

                            if (fitnessEstimator_->compute(C_star) < x &&
                                fitnessEstimator_->compute(Cprim) > x) {
                                delete Cprim;
                                continue;
                            }
                        }

                        new_Rp.push_back(Cprim);
                    }

                    new_Rp.push_back(C_star);
                    Rp = std::move(new_Rp);
                }
            }
            delete C;
        }

        std::vector<Network*> filtered_Rp;
        std::vector<bool> toDelete(Rp.size(), false);

        for (size_t i = 0; i < Rp.size(); ++i) {
            for (size_t j = 0; j < Rp.size(); ++j) {
                if (i != j && !subsumption_->check(Rp[i], Rp[j]).empty()) {
                    toDelete[j] = true;
                }
            }
        }

        for (size_t i = 0; i < Rp.size(); ++i) {
            if (!toDelete[i]) {
                filtered_Rp.push_back(Rp[i]);
            }
            else {
                delete Rp[i];
            }
        }
        Rp = std::move(filtered_Rp);

        while (Rp.size() > static_cast<size_t>(bound)) {
            auto worst = std::max_element(Rp.begin(), Rp.end(), [&](Network* a, Network* b) {
                return fitnessEstimator_->compute(a) < fitnessEstimator_->compute(b);
                });
            delete* worst;
            Rp.erase(worst);
        }

        Rp_prev = Rp;
    }

    return Rp_prev;
}
