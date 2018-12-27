#include <utility>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "vector"
#include "algorithm"
#include "ant_colony_optimization.h"
#include "greedy_algorithm.h"
#include "random_provider.h"
#include "graph.h"

using namespace std;

double **heuristicDistance;
double ROUTE_RESTORE = 0.9;
double **pheromones;
double BETA = 5;
double ALPHA = 1;
double tauMAX;
double RO = 0.1;
double a;
double tauMIN;
vector<int> intCities;

/**
 * Initializes a heuristic matrix depending on the distance of the city from current. Further away cities are given a lower heuristic value to
 * decrease the later probability of picking that city
 * @param size - size of the cities
 * @param distanceMatrix - matrix of distances between cities
 */
void initializeHeuristicDistances(int size, int **distanceMatrix) {
    auto **hm = new double *[size];
    for (int i = 0; i < size; i++) {
        hm[i] = new double[size];
    }
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            hm[i][j] = hm[j][i] = pow(1. / distanceMatrix[i][j], BETA);
        }
    }
    heuristicDistance = hm;
};

/**
 * Initializes pheromones on each edge of the graph. The values are set to default value of taumax which is calculated by the greedy algorithm below.
 * @param size Number of cities
 */
void initializePheromones(int size) {
    auto **ph = new double *[size];

    for (int i = 0; i < size; i++) {
        ph[i] = new double[size];
    }

    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            ph[i][j] = ph[j][i] = tauMAX;
        }
    }
    pheromones = ph;
};

/**
 * The A factor represents the fraction of the tau max with respect to tau min, which is important as they are the maximal and minimal values of the pheromone path.
 * @param size Number of cities
 * @return parameter A calculated
 */
double calculateA(int size) {
    double mi = (size - 1) / (size * (pow(ROUTE_RESTORE, -1. / size) - 1));

    return mi * size;
}

/**
 * Operator needed for comparison between routes, route is in front of the other if it has lower distance.
 *
 * @param other Other route to compare with the current.
 * @return True if the method caller route has a smaller distance.
 */
bool Route::operator<(const Route &other) const {
    return distance < other.distance;
}

/**
 * Returns the best route from the list of routes in the ant colony.
 *
 * @return Best route with smallest distance
 */
Route AntColony::getBestRoute() {
    vector<Route> copy;
    for (const auto &route : routes) {
        copy.push_back(route);
    }
    sort(copy.begin(), copy.end());

    return copy[0];
}

/**
 * Every ant walks a graph and therefore one route is pushed on the route list.
 */
void AntColony::processAnts() {
    for (int i = 0; i < numberOfAnts; i++) {
        routes.push_back(walkGraph());
    }
}

/**
 * The ant makes a path of the graph, and gives it's passed route.
 *
 * @return Route of the ant that walked the graph.
 */
Route AntColony::walkGraph() {
    int start = iRand(0, size - 1);
    vector<int> route;
    vector<bool> visited;

    for (int i = 0; i < size; i++) {
        visited.push_back(i == start);
    }

    double routeDistance = 0;
    int current = start;
    while (true) {
        route.push_back(current);
        if (route.size() == size) break;

        int next = getNextVertex(current, visited);

        visited[next] = true;
        current = next;
    }

    routeDistance = calculateTourDistance(route, distanceMatrix);

    return Route(route, routeDistance);
}

/**
 * Returns true if there are no closest neighbors in the unvisited states.
 *
 * @param current Current city that is visited
 * @param visited List of visited cities so far.
 * @return True if one of the candidates exist and is unvisited on the graph.
 */
bool AntColony::noMoreCandidates(int current, vector<bool> visited) {
    for (int candidate : getNeighbours(current)) {
        if (!visited[candidate]) return false;
    }
    return true;
}

/**
 * Sorting function that sorts the elements of a pair by the value in descending order.
 *
 * @param a Pair a
 * @param b Pair b
 * @return true if the second value is greater for the first pair
 */
bool reverseSortByDistance(const pair<int, double> &a, const pair<int, double> &b) {
    return (a.second > b.second);
}

/**
 * If there are candidates unvisited in the candidate list for a certain city, probabilities for each city in the
 * candidate list are assigned, and the roulette wheel selection is applied.
 *
 * @param current Current visiting city
 * @param visited List of visited cities.
 * @return next city to be visited
 */
int AntColony::nextCandidate(int current, vector<bool> visited) {
    if (noMoreCandidates(current, visited)) return -1;

    vector<pair<int, double>> probabilities = getProbabilities(current, getNeighbours(current), visited);
    sort(probabilities.begin(), probabilities.end(), reverseSortByDistance);

    double probability = fRand();

    for (auto &it : probabilities) {
        probability -= it.second;

        if (probability <= 0) {
           return it.first;
        }
    }
    return -1;
}

/**
 * Returns the next candidate for the city to be visited. First the list of candidates is explored and picked between
 * them, after that the other possibilities are included. Roulette wheel is done for the selection of the cities.
 *
 * @param current Current visiting city
 * @param visited List of visited cities
 * @return next city to be visited
 */
int AntColony::getNextVertex(int current, vector<bool> visited) {
    int nc = nextCandidate(current, visited);

    if (nc != -1) {
        return nc;
    }

    vector<pair<int, double>> probabilities = getProbabilities(current, intCities, visited);
    sort(probabilities.begin(), probabilities.end(), reverseSortByDistance);
    double probability = fRand();

    for (auto it = probabilities.begin(); it != probabilities.end(); ++it) {
        probability -= it->second;

        if (probability <= 0) {
            return it->first;
        }
    }
    return -1;
}

/**
 * Gets the denominator that is the sum of all probabilities, to normalize it.
 * @param current Current visiting city
 * @param visited List of visited cities
 * @return Normalization factor for the probabilities
 */
double AntColony::getDenominator(int current, vector<bool> visited, vector<int> candidates) {
    double sum = 0;

    for (int vertex : candidates) {
        if (visited[vertex]) continue;
        sum += getProbability(current, vertex);
    }
    return sum;
}

/**
 * Gets the probabilities for each city of the candidates list, that can or can't be visited.
 * @param current current visiting city
 * @param candidateList list of possible candidates for visiting
 * @param visited list of visited cities
 * @return probabilities for each city
 */
vector<pair<int, double>> AntColony::getProbabilities(int current, vector<int> candidateList, vector<bool> visited) {
    vector<pair<int, double>> probabilities;
    double denominator = getDenominator(current, visited, candidateList);

    for (int &c : candidateList) {
        if (visited[c]) {
            probabilities.emplace_back(make_pair(c, 0));
        } else {
            probabilities.emplace_back(make_pair(c, getProbability(current, c) / denominator));
        }
    }

    return probabilities;
}

/**
 * Returns the probability of an ant to go along that path based on the current pheromone track powered by alpha and
 * heuristic powered by beta.
 *
 * @param current current city visited
 * @param v second city
 * @return probability to go along that path
 */
double AntColony::getProbability(int current, int v) {
    return pow(pheromones[current][v], ALPHA) * heuristicDistance[current][v];
}

/**
 * Minimal and maximal values for the pheromone path are updated
 *
 * @param distance Taumin and Taumax are calculated based on the distance provided, reversely proportional. They tend to
 * get better as the taumax is closer to the optimal solution.
 */
void updateMinMaxParameters(double distance) {
    tauMAX = 1. / (RO * distance);
    tauMIN = tauMAX / a;
}

/**
 * Updates the pheromones on the path, watching for the highest amount of tauMAX.
 * @param ti City 1
 * @param tj City 2
 * @param d Amount to update the pheromone path
 */
void update(int ti, int tj, double d) {
    double level = pheromones[ti][tj] + d < tauMAX ? pheromones[ti][tj] + d : tauMAX;
    pheromones[ti][tj] = pheromones[tj][ti] = level;
}

/**
 * Updates the pheromones, the smallest amount is tauMIN.
 * @param best Best route that updates the pheromone path.
 */
void updatePheromoneLevels(Route best, int **distanceMatrix, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = i; j < size; j++) {
            double possibleLevel = (1 - RO) * pheromones[i][j];
            double level = possibleLevel > tauMIN ? possibleLevel : tauMIN;
            pheromones[i][j] = pheromones[j][i] = level;
        }
    }

    int n = (int)best.vertices.size();
    for (int i = 0; i < n; i++) {
        int ti = best.vertices[i];
        int tj = best.vertices[(i + 1) % n];
        double d = 1. / distanceMatrix[ti][tj];
        update(ti, tj, d);
    }
}

/**
 * Duplicates one route, creating a copy of another.
 * @param r Route to be copied
 * @return Copied route
 */
Route* Route::duplicate(Route r) {
    Route *result = new Route(r.vertices, r.distance);
    return result;
}

/**
 * Runs the ant colony algorithm, iterating for a certain number of iterations with certain number of ants.
 * First every ant walks on the graph and places pheromones, then the pheromones evaporate and the route is updated
 * with the best route
 *
 * @param noOfCandidates number of cities in the candidate list.
 * @param l Number of ants
 * @param maxiter  Number of iterations
 * @return Best route found.
 */
vector<int> AntColonyOptimization::run(int noOfCandidates, int l, int maxiter) {

    for (int i = 0; i < size; i++) {
        intCities.push_back(i);
    }

    //Initialization of all matrices and lists
    initializeHeuristicDistances(size, distanceMatrix);
    initializeCandidatesList(noOfCandidates, distanceMatrix, size);
    initializeParameters();
    initializePheromones(size);

    //Global best route
    globalBest = nullptr;
    int changeCounter = 0;
    for (int i = 0; i < maxiter; i++) {
        //All ants make its path
        AntColony colony(l, distanceMatrix, size);
        colony.processAnts();
        Route iterBest = colony.getBestRoute();

        //The tau min and tau max are updated with a better version
        if (globalBest == nullptr || iterBest < *globalBest) {
            globalBest = iterBest.duplicate(iterBest);
            updateMinMaxParameters(globalBest->distance);
            changeCounter = 0;
        }


        if (changeCounter > 0.2 * maxiter) {
            changeCounter = 0;
            initializePheromones(size);
        }

        //Updating the pheromone levels each time
        updatePheromoneLevels(*globalBest, distanceMatrix, size);
        changeCounter++;
    }

    vector<int> v;
    for (auto &vertex : globalBest->vertices) {
        v.push_back(vertex);
    }
    return v;
};

/**
 * A good tauMAX would be a greedy approach for a start
 * Taumin is the least number for pheromones
 */
void AntColonyOptimization::initializeParameters() {
    double greedy = calculateTourDistance(greedyAlgorithm(distanceMatrix, size), distanceMatrix);
    tauMAX = 1. / (RO * greedy);
    a = calculateA(size);
    tauMIN = tauMAX / a;
}


/**
 * Starts the min max ant system algorithm
 */
vector<int> antColonySolver(int **distanceMatrix, int size) {
    int candidateList = (int) (size / 5);
    int numberOfAnts = 30;
    int maxiter = 100;


    AntColonyOptimization a(distanceMatrix, size);
    vector<int> route = a.run(candidateList, numberOfAnts, maxiter);

    return route;
}