#include <vector>
#include "graph.h"
#include "opt2local.h"

using namespace std;

/**
 * Algorithm performing the 2-opt optimization till solution doesn't change
 *
 * @param path Input into algorithm
 * @param distanceMatrix Distance matrix of TSP problem
 * @param size Size of TSP problem
 * @return Route after performing several 2-opt optimizations.
 */
vector<int> opt2Algorithm(vector<int> path, int **distanceMatrix, int size) {
    if (size <= 3) return path;
    int bestDistance = calculateTourDistance(path, distanceMatrix);
    vector<int> best = vector<int>(path);
    vector<int> v = vector<int>(path);

    while (true) {
        v.push_back(v[0]);
        for (int i = 0; i < size - 3; i++) {
            for (int j = i + 2; j < size; j++) {
                if (distanceMatrix[v[i]][v[i + 1]] + distanceMatrix[v[j]][v[j + 1]] >
                    distanceMatrix[v[i]][v[j]] + distanceMatrix[v[i + 1]][v[j + 1]]) {
                    for (int k = 0; k < (j - i) / 2; k++) {
                        int a = v[j - k];
                        v[j - k] = v[i + k + 1];
                        v[i + k + 1] = a;
                    }
                }
            }
        }
        v.pop_back();
        int distance = calculateTourDistance(v, distanceMatrix);
        if (distance >= bestDistance) break;
        best = vector<int>(v);
        bestDistance = distance;
    }
    return best;
}

/**
 * Faster computing 2-OPT by only looking to the nearest neighbors
 * # NOT USED
 *
 * @param path Input into algorithm
 * @param distanceMatrix Distance matrix of TSP problem
 * @param size Size of TSP problem
 * @return Route after performing several 2-opt optimizations.
 */
vector<int> opt2Speeded(vector<int> path, int **distanceMatrix, int size) {
    if (size <= 3) return path;
    int bestDistance = calculateTourDistance(path, distanceMatrix);
    vector<int> best = vector<int>(path);
    vector<int> v = vector<int>(path);

    while (true) {
        v.push_back(v[0]);
        for (int i = 0; i < size - 1; i++) {
            for (int j : getNeighbours(i + 1)) {
                if (distanceMatrix[i + 1][j] >= distanceMatrix[i][i + 1]) {
                    for (int k = 0; k < (j - i) / 2; k++) {
                        int a = v[j - k];
                        v[j - k] = v[i + k + 1];
                        v[i + k + 1] = a;
                    }
                }
            }
        }
        v.pop_back();
        int distance = calculateTourDistance(v, distanceMatrix);
        if (distance >= bestDistance) break;
        best = vector<int>(v);
        bestDistance = distance;
    }
    return best;
}