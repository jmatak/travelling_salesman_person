#ifndef TSP_FINAL_ANT_COLONY_OPTIMIZATION_H
#define TSP_FINAL_ANT_COLONY_OPTIMIZATION_H

#include "vector"
#include "map"
#include "graph.h"

using namespace std;

void initializeHeuristicDistances(int size, int **distanceMatrix);

void initializePheromones(int size);

double calculateA(int size);

class Route {
public:
    vector<int> vertices;
    double distance;

    bool operator<(const Route &other) const;

    Route(vector<int> vertices, double distance) : vertices(vertices), distance(distance) {};

    Route() {};

    Route *duplicate(Route r);
};

class AntColony {
public:
    vector<Route> routes;
    int numberOfAnts;
    int **distanceMatrix;
    int size;

    AntColony(int numberOfAnts, int **distanceMatrix, int size) : numberOfAnts(numberOfAnts), distanceMatrix(distanceMatrix), size(size) {};

    Route getBestRoute();

    void processAnts();

    Route walkGraph();

    bool noMoreCandidates(int current, vector<bool> visited);

    int nextCandidate(int current, vector<bool> visited);

    int getNextVertex(int current, vector<bool> visited);

    double getDenominator(int current, vector<bool> visited, vector<int> candidates);

    vector<pair<int, double>> getProbabilities(int current, vector<int> candidateList, vector<bool> visited);

    double getProbability(int current, int v);
};

void updateMinMaxParameters(double distance);

void update(int ti, int tj, double d);

void updatePheromoneLevels(Route best, int **distanceMatrix, int size);

class AntColonyOptimization {
public:
    Route *globalBest;
    int **distanceMatrix;
    int size;

    AntColonyOptimization(int **distanceMatrix, int size) : distanceMatrix(distanceMatrix), size(size) {};

    vector<int> run(int noOfCandidates, int l, int maxiter);

    void initializeParameters();
};

vector<int> antColonySolver(int **distanceMatrix, int size);

#endif //TSP_FINAL_ANT_COLONY_OPTIMIZATION_H
