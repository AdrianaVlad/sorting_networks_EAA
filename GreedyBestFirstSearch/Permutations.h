#pragma once

#include <vector>

class Permutations {
public:
    static const int MAX_N = 12;

    static const std::vector<std::vector<std::vector<int>>>& getAll();
    static const std::vector<std::vector<int>>& get(int n);
    static const std::vector<int>& identity(int n);
    static int factorial(int n);

private:
    static std::vector<std::vector<std::vector<int>>> PERM;
    static std::vector<bool> used;
    static int count;

    static const int FACT[13];

    static void init();
    static void createIdentity(int n);
    static void createRec(int n, int len);
    static void create(int n);

    static bool initialized;
};
