#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

/**
 * Function for obtaining index of minimum cut between unused vertices and currently selected index.
 *
 * @param mins Array defining current distances between vertices and currently selected
 * @param used Array defining if vertex has already been used
 * @param size Size of arrays
 * @return Index that corresponds to minimal distance
 */
int minimumIndex(const int mins[], const int used[], int size) {
    int minIndex = -1;
    for (int i = 0; i < size; ++i) {
        if (!used[i] && (mins[i] < mins[minIndex] || minIndex == -1)) {
            minIndex = i;
        }
    }
    return minIndex;
}

/**
 * Prim's algorithm for finding minimum spanning tree. Finding a cut in every step of algorithm to connect vertices
 * already included in MST and those who are not. That guarantees creating a minimum spanning tree.
 *
 * @param mst Vector of edges defining minimum spanning tree
 * @param dist Matrix of distances between vertices
 * @param size Size of TSP problem
 */
void minimumSpanningTree(vector<pair<int, int>> &mst, int **dist, int size) {
    // Distances from current vertex and ones to be included in MST
    int mins[size];
    // Array for defining if vertex is used in MST
    int used[size] = {0};
    // Array corresponding to joined vertices
    int edge[size] = {0};

    for (int i = 0; i < size; ++i) {
        if (i != 0) mins[i] = INT32_MAX;
        else mins[i] = 0;
    }
    for (int i = 0; i < size - 1; ++i) {
        // Finding current minimum index, closest disjoint edge
        int index = minimumIndex(mins, used, size);
        used[index] = 1;
        for (int j = 0; j < size; ++j) {
            if (j == index) continue;
            // Setting new values for vertices not yet included in MST
            if (!used[j] && dist[index][j] < mins[j]) {
                edge[j] = index;
                mins[j] = dist[index][j];
            }
        }
    }
    // Every edge is defined with index and it's corresponding in that moment closest not used vertex
    for (int i = 1; i < size; ++i) {
        mst.push_back(make_pair(i, edge[i]));
    }
}


/**
 *  Greedy algorithm adapted for finding TSP tour through edges that have odd number of connected edges.
 *
 * @param graph Vertices with odd number of edges
 * @param distanceMatrix Matrix defining distances in graph
 * @return Greedy route through vertices with odd number of edges
 */
vector<int> greedyTourMatching(vector<int> graph, int **distanceMatrix) {
    vector<int> tour;
    int size = (int) graph.size();
    int used[size] = {0};
    tour.push_back(graph[0]);

    // Greedy algorithm adapted for tour matching
    used[0] = 1;
    for (int i = 1; i < size; i++) {
        int best = -1;
        int bestInd = -1;
        for (int j = 0; j < size; j++) {
            if (!used[j] && (best == -1 || distanceMatrix[tour[i - 1]][graph[j]] < distanceMatrix[tour[i - 1]][best])) {
                best = graph[j];
                bestInd = j;
            }
        }
        tour.push_back(best);
        used[bestInd] = 1;
    }

    return tour;
}

/**
 *  Function for matching vertices with odd number of joined edges. Algorithm used is greedy. Greedy TSP algorithm is
 *  used to obtain route through vertices with odd number of edges. After that step, every pair of vertices is then
 *  connected to create even number of edges in each vertex, since that is prerequisite for Euler graph.
 *
 * @param mst Provided minimum spanning tree
 * @param distanceMatrix Matrix defining distances in graph
 * @param size Size of TSP problem
 */
void greedyMinWeightMatching(vector<pair<int, int>> &mst, int **distanceMatrix, int size) {
    // Calculation of vertices degree
    int histogram[size] = {0};
    for (pair<int, int> e : mst) {
        histogram[e.first]++;
        histogram[e.second]++;
    }

    // Collecting tour with odd vertices degree
    vector<int> oddTour;
    for (int i = 0; i < size; ++i) {
        if (histogram[i] % 2 == 0) continue;
        oddTour.push_back(i);
    }

    // Shuffling tour for get different starting position (since greedy alg is sensible to starting position)
    shuffle(oddTour.begin(), oddTour.end(), std::mt19937(std::random_device()()));
    vector<int> match = greedyTourMatching(oddTour, distanceMatrix);

    // Collecting pairs in TSP tour obtained with greedy algorithm
    for (int j = 1; j < match.size(); j += 2) {
        mst.emplace_back(match[j], match[j - 1]);
    }
}

/**
 * With edges that define minimum spanning tree, first step is creation of euler graph, with every vertex to
 * have even degree. Array provided in function defines vertices of Euler graph. Function then finds cycles of vertices
 * provided as input. That task can always be done when euler's graph is given.
 *
 * @param mst Minimum spanning tree with tour matching
 * @param distanceMatrix Matrix defining distances in graph
 * @param size Size of TSP problem
 * @return Cycles which should be connected
 */
vector<vector<int>> eulerCycles(vector<pair<int, int>> mst, int **distanceMatrix, int size) {
    // Keeping track of visited vertices
    int visited[size] = {0};
    for (pair<int, int> e : mst) {
        visited[e.first]++;
        visited[e.second]++;
    }
    int searching = mst[0].first;

    vector<vector<int>> cycles;
    vector<int> vertices;
    for (int i = 0, n = (int) mst.size(); i < n; ++i) {

        //Creating one cycle
        int index = -1;
        vertices.push_back(searching);

        for (int j = 0; j < mst.size(); ++j) {
            // After matching search for second vertex in pair
            if (mst[j].first == searching) {
                index = j;
                searching = mst[j].second;
                break;
            } else if (mst[j].second == searching) {
                index = j;
                searching = mst[j].first;
                break;
            }
        }
        if (index == -1) {
            // After closing cycle, add vertices
            cycles.push_back(vertices);
            vertices.clear();

            for (int k = 0; k < size; k++) {
                if (visited[k] == 2) {
                    // Next cycle starts at unvisited
                    searching = k;
                    break;
                }
            }
            --i;
            continue;
        }

        // Show visited indexes and delete visited edge
        visited[mst[index].first]--;
        visited[mst[index].second]--;
        mst.erase(mst.begin() + index);
    }
    cycles.push_back(vertices);

    return cycles;
}

/**
 * Function for checking if value is contained in vector. If that is the case, index of value is returned, -1 otherwise.
 *
 * @param v Vector to search in
 * @param s Value that should be found
 * @return Index of value if contains, -1 otherwises
 */
int containsIndex(vector<int> v, int s) {
    for (int i = 0, n = v.size(); i < n; ++i) {
        if (s == v[i]) return i;
    }
    return -1;
}

/**
 * Deleting created duplicates to create hamiltonian tour around graph.
 *
 * @param tour Solution in which duplicates will be removed
 */
void deleteDuplicates(vector<int> &tour) {
    for (int i = 0; i < tour.size(); ++i) {
        vector<int> remove;
        for (int j = i + 1; j < tour.size(); ++j) {
            if (tour[i] == tour[j]) {
                remove.push_back(j);
            }
        }
        for (int j = 0; j < remove.size(); ++j) {
            tour.erase(tour.begin() + remove[j] - j);
        }
    }
}

/**
 * Provided with disjoint cycles, function for connecting those cycles and creating Hamiltonian cycle around graph. This
 * task is always possible since every disjoint cycle shares one index with other cycle.
 *
 * @param cycles Provided disjoint cycles
 * @param distanceMatrix Matrix defining distances in graph
 * @param size Size of TSP problem
 * @return Hamiltonian cycle
 */
vector<int> hamiltonianPath(vector<vector<int>> cycles, int **distanceMatrix, int size) {
    vector<int> tour;
    // First euclidian cycle
    tour.insert(tour.end(), cycles[0].begin(), cycles[0].end());
    cycles.erase(cycles.begin());
    for (int i = 0; i < tour.size(); ++i) {
        int index = -1;
        // Find cycle that contains vertex with same index
        for (int j = 0; j < cycles.size(); ++j) {
            index = containsIndex(cycles[j], tour[i]);
            if (index != -1) {
                for (int k = 1; k < cycles[j].size() + 1; ++k) {
                    tour.insert(tour.begin() + i + k, cycles[j][(k + index) % cycles[j].size()]);
                }

                // Delete added cycle
                cycles.erase(cycles.begin() + j);
                break;
            }
        }
        if (cycles.empty()) break;
    }


    deleteDuplicates(tour);
    return tour;
}

/**
 * Core of the Christofides algorithm, first MST is created using Prim's greedy method. After that, vertices with odd
 * degree are connected with greedy tour matching. Last step is, from the obtained euler's graph get disjoint cycles
 * and connect them into one Hamiltonian cylce.
 *
 * @param distanceMatrix Matrix defining distances in graph
 * @param size Size of TSP problem
 * @return Hamiltonian path that is output from Christofides algorithm
 */
vector<int> christofidesAlgorithm(int **distanceMatrix, int size) {
    vector<pair<int, int>> mst;
    minimumSpanningTree(mst, distanceMatrix, size);
    greedyMinWeightMatching(mst, distanceMatrix, size);
    vector<vector<int>> eCycles = eulerCycles(mst, distanceMatrix, size);

    return hamiltonianPath(eCycles, distanceMatrix, size);
}
