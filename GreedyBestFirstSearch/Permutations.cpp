#include "Permutations.h"
#include "Config.h"
#include <algorithm>
#include <iostream>

const int Permutations::FACT[13] = {
    1, 1, 2, 6, 24, 120, 720,
    5040, 40320, 362880, 3628800, 39916800, 479001600
};

std::vector<std::vector<std::vector<int>>> Permutations::PERM;
std::vector<bool> Permutations::used;
int Permutations::count = 0;
bool Permutations::initialized = false;

void Permutations::init() {
    if (initialized) return;
    initialized = true;

    int n = Config::getMaxNbWires();

    PERM.resize(n + 1);
    for (int i = 1; i <= n; ++i) {
        createIdentity(i);
        // create(i);
    }
}

void Permutations::createIdentity(int n) {
    PERM[n].resize(1);
    PERM[n][0].resize(n);
    for (int i = 0; i < n; ++i) {
        PERM[n][0][i] = i;
    }
}

void Permutations::create(int n) {
    int total = FACT[n];
    PERM[n].resize(total, std::vector<int>(n, 0));
    used.assign(n, false);
    count = 0;
    createRec(n, 0);
}

void Permutations::createRec(int n, int len) {
    for (int i = 0; i < n; ++i) {
        if (used[i]) continue;
        PERM[n][count][len] = i;
        used[i] = true;
        if (len < n - 1) {
            createRec(n, len + 1);
        }
        else if (count < static_cast<int>(PERM[n].size()) - 1) {
            count++;
            PERM[n][count] = PERM[n][count - 1];
        }
        used[i] = false;
    }
}

const std::vector<std::vector<std::vector<int>>>& Permutations::getAll() {
    if (!initialized) init();
    return PERM;
}

const std::vector<std::vector<int>>& Permutations::get(int n) {
    if (!initialized) init();
    return PERM[n];
}

const std::vector<int>& Permutations::identity(int n) {
    if (!initialized) init();
    return PERM[n][0];
}

int Permutations::factorial(int n) {
    return FACT[n];
}
