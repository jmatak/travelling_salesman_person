#include "vector"

using namespace std;

/**
 * Simple greedy nearest neighbor algorithm presented in project.
 *
 * @param dm Distance matrix
 * @param size Size of TSP problem
 * @return Solution of greedy nearest neighbor algorithm
 */
vector<int> greedyAlgorithm(int **dm, int size) {
    vector<int> tour;
    int used[size] = {0};
    tour.push_back(0);
    used[0] = 1;

    for (int i = 1; i < size; i++) {
        int best = -1;
        for (int j = 0; j < size; j++) {
            if (!used[j] && (best == -1 || dm[i - 1][j] < dm[i - 1][best])) {
                best = j;
            }
        }
        tour.push_back(best);
        used[best] = 1;
    }
    return tour;

}