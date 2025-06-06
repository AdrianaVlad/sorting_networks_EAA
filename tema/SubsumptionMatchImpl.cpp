#include "SubsumptionMatchImpl.h"
#include "SubsumptionVerifier.h"
#include "OutputCluster.h"

SubsumptionMatchImpl::SubsumptionMatchImpl() {}

std::vector<int> SubsumptionMatchImpl::findPermutation(const OutputSet& out0, const OutputSet& out1) {
    int nbWires = out0.getNbWires();
    std::vector<std::vector<int>> graph(nbWires, std::vector<int>(nbWires, 0));
    std::vector<std::vector<int>> degrees(2, std::vector<int>(nbWires, 0));

    for (int u = 0; u < nbWires; ++u) {
        for (int v = 0; v < nbWires; ++v) {
            for (int k = 1; k < nbWires; ++k) {
                const OutputCluster* c0 = out0.cluster(k);
                const OutputCluster* c1 = out1.cluster(k);

                if ((c0->getPos0()[u] && !c1->getPos0()[v]) ||
                    (c0->getPos1()[u] && !c1->getPos1()[v]) ||
                    (c0->size() == c1->size() &&
                        ((c1->getPos0()[v] && !c0->getPos0()[u]) ||
                            (c1->getPos1()[v] && !c0->getPos1()[u])))) {
                    goto skip;
                }
            }
            graph[u][v] = 1;
            degrees[0][u]++;
            degrees[1][v]++;
        skip:;
        }
        if (degrees[0][u] == 0) return {};
    }

    for (int w = 0; w < nbWires; ++w) {
        if (degrees[1][w] == 0) return {};
    }

    return checkMatchings(out0, out1, graph, degrees);
}

std::vector<int> SubsumptionMatchImpl::checkMatchings(const OutputSet& out0, const OutputSet& out1,
    std::vector<std::vector<int>>& graph,
    std::vector<std::vector<int>>& degrees) {

    auto matching = findMatching(graph);
    if (matching.empty()) return {};

    if (checkPermutation(out0, out1, matching[0])) {
        return matching[0];
    }

    return checkMatchingsRec(out0, out1, graph, degrees, matching);
}

std::vector<std::vector<int>> SubsumptionMatchImpl::findMatching(const std::vector<std::vector<int>>& graph) {
    int n = graph.size();
    std::vector<std::vector<int>> matching(2, std::vector<int>(n, -1));

    for (int u = 0; u < n; ++u) {
        std::vector<bool> seen(n, false);
        if (!matchRec(graph, u, 0, seen, matching)) {
            return {};
        }
    }

    return matching;
}

bool SubsumptionMatchImpl::matchRec(const std::vector<std::vector<int>>& graph, int u, int next,
    std::vector<bool>& seen, std::vector<std::vector<int>>& matching) {
    int n = graph.size();
    for (int v = next; v < n; ++v) {
        if (graph[u][v] == 1 && matching[1][v] == -1) {
            matching[0][u] = v;
            matching[1][v] = u;
            return true;
        }
    }
    for (int v = next; v < n; ++v) {
        if (graph[u][v] == 1 && !seen[v] && matching[1][v] >= 0) {
            seen[v] = true;
            if (matchRec(graph, matching[1][v], 0, seen, matching)) {
                matching[0][u] = v;
                matching[1][v] = u;
                return true;
            }
        }
    }
    return false;
}

std::vector<int> SubsumptionMatchImpl::checkMatchingsRec(const OutputSet& out0, const OutputSet& out1,
    std::vector<std::vector<int>>& graph,
    std::vector<std::vector<int>>& degrees,
    std::vector<std::vector<int>>& matching) {

    int n = graph.size();
    auto cycle = findCycle(graph, degrees, matching);
    if (cycle.empty()) return {};

    std::vector<std::vector<int>> mprime = matching;
    int i = 1;
    while (i > 0) {
        int v = cycle[i];
        int w = (i + 1 < cycle.size()) ? cycle[i + 1] : cycle[0];
        if (i + 1 < cycle.size()) i += 2;
        else i = 0;
        mprime[1][v] = w;
        mprime[0][w] = v;
    }

    if (checkPermutation(out0, out1, mprime[0])) {
        return mprime[0];
    }

    int u = cycle[0];
    int v = cycle[1];

    graph[u][v] = 0;
    degrees[0][u]--;
    degrees[1][v]--;

    auto perm = checkMatchingsRec(out0, out1, graph, degrees, mprime);
    if (!perm.empty()) return perm;

    graph[u][v] = 1;
    degrees[0][u]++;
    degrees[1][v]++;

    for (int w = 0; w < n; ++w) {
        if (w != v && graph[u][w] > 0) {
            if (graph[u][w] == 1) {
                degrees[0][u]--;
                degrees[1][w]--;
            }
            graph[u][w]++;
        }
        if (w != u && graph[w][v] > 0) {
            if (graph[w][v] == 1) {
                degrees[0][w]--;
                degrees[1][v]--;
            }
            graph[w][v]++;
        }
    }

    bool hasEmpty = false;
    for (int w = 0; w < n; ++w) {
        if (degrees[0][w] == 0 || degrees[1][w] == 0) {
            hasEmpty = true;
            break;
        }
    }

    if (!hasEmpty) {
        perm = checkMatchingsRec(out0, out1, graph, degrees, matching);
        if (!perm.empty()) return perm;
    }

    for (int w = 0; w < n; ++w) {
        if (w != v && graph[u][w] > 1) {
            graph[u][w]--;
            if (graph[u][w] == 1) {
                degrees[0][u]++;
                degrees[1][w]++;
            }
        }
        if (w != u && graph[w][v] > 1) {
            graph[w][v]--;
            if (graph[w][v] == 1) {
                degrees[0][w]++;
                degrees[1][v]++;
            }
        }
    }

    return {};
}

std::vector<int> SubsumptionMatchImpl::findCycle(const std::vector<std::vector<int>>& graph,
    const std::vector<std::vector<int>>& degrees,
    const std::vector<std::vector<int>>& matching) {

    int n = graph.size();
    for (int u = 0; u < n; ++u) {
        if (degrees[0][u] < 2) continue;

        std::vector<std::vector<int>> visited(2, std::vector<int>(n, 0));
        visited[0][u] = 1;
        auto cycle = findCycleRec(graph, degrees, matching, u, 1, visited);
        if (!cycle.empty()) return cycle;
    }
    return {};
}

std::vector<int> SubsumptionMatchImpl::findCycleRec(const std::vector<std::vector<int>>& graph,
    const std::vector<std::vector<int>>& degrees,
    const std::vector<std::vector<int>>& matching,
    int u, int pos,
    std::vector<std::vector<int>>& visited) {

    int n = graph.size();
    int set = (pos - 1) % 2;

    if (set == 1) {
        for (int v = 0; v < n; ++v) {
            if (graph[v][u] != 1 || matching[1][u] == v || visited[0][v] <= 0) continue;

            int first = visited[0][v];
            std::vector<int> cycle(pos - first + 1);
            for (int i = 0; i < n; ++i) {
                if (visited[0][i] >= first) cycle[visited[0][i] - first] = i;
                if (visited[1][i] >= first) cycle[visited[1][i] - first] = i;
            }
            return cycle;
        }
    }

    for (int v = 0; v < n; ++v) {
        if (visited[1 - set][v] > 0 || degrees[1 - set][v] < 2) continue;

        bool edgeValid = (set == 0 && graph[u][v] == 1 && matching[set][u] == v) ||
            (set == 1 && graph[v][u] == 1 && matching[set][u] != v);

        if (!edgeValid) continue;

        visited[1 - set][v] = pos + 1;
        auto cycle = findCycleRec(graph, degrees, matching, v, pos + 1, visited);
        if (!cycle.empty()) return cycle;

        visited[1 - set][v] = 0;
    }

    return {};
}

bool SubsumptionMatchImpl::checkPermutation(const OutputSet& out0, const OutputSet& out1, const std::vector<int>& perm) {
    if (Statistics::ENABLED) {
        Statistics::permTotal++;
    }
    return Subsumption::checkPermutation(out0, out1, perm);
}
