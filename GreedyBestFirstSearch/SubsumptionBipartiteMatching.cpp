#include "SubsumptionBipartiteMatching.h"

void SubsumptionBipartiteMatching::buildSubsumptionGraph(const OutputSet& out0, const OutputSet& out1) {
    n = out0.getNbWires();
    graph.assign(n, std::vector<bool>(n, true));

    for (int p = 0; p <= n; ++p) {
        const OutputCluster* c0 = out0.cluster(p);
        const OutputCluster* c1 = out1.cluster(p);
        if (!c0 || !c1) continue;

        for (int i = 0; i < n; ++i) {
            if (c0->getPos0()[i]) {
                for (int j = 0; j < n; ++j)
                    if (!c1->getPos0()[j]) graph[i][j] = false;
            }
            if (c0->getPos1()[i]) {
                for (int j = 0; j < n; ++j)
                    if (!c1->getPos1()[j]) graph[i][j] = false;
            }

            if (c0->size() == c1->size()) {
                for (int j = 0; j < n; ++j) {
                    if (c1->getPos0()[j] && !c0->getPos0()[i]) graph[i][j] = false;
                    if (c1->getPos1()[j] && !c0->getPos1()[i]) graph[i][j] = false;
                }
            }
        }
    }
}

bool SubsumptionBipartiteMatching::findInitialMatching(std::vector<int>& matchTo) {
    matchTo.assign(n, -1);
    std::vector<bool> visited;

    std::function<bool(int)> bpm = [&](int u) {
        for (int v = 0; v < n; ++v) {
            if (graph[u][v] && !visited[v]) {
                visited[v] = true;
                if (matchTo[v] < 0 || bpm(matchTo[v])) {
                    matchTo[v] = u;
                    return true;
                }
            }
        }
        return false;
        };

    for (int u = 0; u < n; ++u) {
        visited.assign(n, false);
        if (!bpm(u)) return false;
    }

    return true;
}

void SubsumptionBipartiteMatching::enumerateMatchings(std::vector<int>& matchTo,
    std::function<bool(const std::vector<int>&)> callback) {

    std::vector<int> match(n, -1);
    for (int j = 0; j < n; ++j)
        if (matchTo[j] >= 0) match[matchTo[j]] = j;

    std::vector<bool> used(n, false);
    dfsEnumerate(match, used, 0, callback);
}

void SubsumptionBipartiteMatching::dfsEnumerate(std::vector<int>& match,
    std::vector<bool>& used, int u,
    std::function<bool(const std::vector<int>&)> callback) {

    if (u == n) {
        if (callback(match)) return;
        return;
    }

    for (int v = 0; v < n; ++v) {
        if (graph[u][v] && !used[v]) {
            match[u] = v;
            used[v] = true;
            dfsEnumerate(match, used, u + 1, callback);
            used[v] = false;
        }
    }
}

std::vector<int> SubsumptionBipartiteMatching::findPermutation(const OutputSet& out0, const OutputSet& out1) {
    buildSubsumptionGraph(out0, out1);

    std::vector<int> matchTo;
    if (!findInitialMatching(matchTo))
        return {};

    std::vector<int> result;
    enumerateMatchings(matchTo, [&](const std::vector<int>& perm) -> bool {
        if (checkPermutation(out0, out1, perm)) {
            result = perm;
            return true;
        }
        return false;
        });

    return result;
}
