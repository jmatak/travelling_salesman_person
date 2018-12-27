#include <vector>
#include <math.h>
#include <algorithm>
#include "map"
#include "graph.h"

using namespace std;


/**
 * Calculating euclidian distance between 2 points in space defined as Vertex(x,y). Distance is then rounded
 * to closest integer.
 *
 * @param v1 First vertex
 * @param v2 Second vertex
 * @return Distance between two vertices
 */
int calculateDistance(Vertex v1, Vertex v2) {
    double dx = v1.x - v2.x;
    double dy = v1.y - v2.y;

    return static_cast<int>(round(sqrt(dx * dx + dy * dy)));
}


/**
 * Function used to calculate distance of route provided as input.
 *
 * @param solution Route for calculating total tour distance
 * @param distanceMatrix Matrix used for obtaining distances
 * @return Total tour distance
 */
int calculateTourDistance(vector<int> solution, int **distanceMatrix) {
    int sum = distanceMatrix[solution[0]][solution[solution.size() - 1]];
    for (int i = 1; i < (int) solution.size(); ++i) {
        sum += distanceMatrix[solution[i]][solution[i - 1]];
    }
    return sum;
}

map<int, vector<int>> neighborhood;

/**
 * Function for sorting by distance if input is pair of index and distance
 *
 * @param a Index
 * @param b Distance
 * @return True if first is closer
 */
bool sortByDistance(const pair<int, double> &a, const pair<int, double> &b) {
    return (a.second < b.second);
}

/**
 * Returns neighbor from specific position
 *
 * @param i index of neighbor
 * @return Index of neighbor
 */
vector<int> getNeighbours(int i) {
    return neighborhood[i];
}

/**
 * Finding n nearest neighbors of certain city
 *
 * @param index Index of searched city
 * @param noCandidates Number of closest cities we want to obtain
 * @param distanceMatrix Distance matrix
 * @param size Size of TSP tour
 * @return Neighbors of certain city
 */
vector<int> findNeighbors(int index, int noCandidates, int **distanceMatrix, int size) {
    vector<pair<int, double>> distances;

    for (unsigned long i = 0; i < size; i++) {
        if (i == index) continue;
        distances.emplace_back(i, distanceMatrix[index][i]);
    }
    sort(distances.begin(), distances.end(), sortByDistance);
    int min = noCandidates < (size - 1) ? noCandidates : (size - 1);
    vector<int> neighbors(static_cast<unsigned long>(min));
    for (int j = 0; j < min; ++j) {
        neighbors[j] = distances[j].first;
    }
    return neighbors;
}

/**
 * Initialize list of candidates
 * @param k Number of closest neighbors
 */
void initializeCandidatesList(int k, int **distanceMatrix, int size) {
    for (int i = 0; i < size; i++) {
        neighborhood[i] = findNeighbors(i, k, distanceMatrix, size);
    }
}