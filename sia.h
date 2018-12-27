//
// Created by jmatak on 23.10.18..
//

#ifndef TSP_FINAL_CLONALG_H
#define TSP_FINAL_CLONALG_H

#include <vector>
#include <random>
#include <algorithm>
#include "opt2local.h"
#include "graph.h"

using namespace std;


typedef pair<vector<int>, int> Individual;

Individual createNeighbor(vector<int> solution, int **dm);

vector<Individual> initializeSolutions(const vector<int> &solution, int popSize, int **dm);

bool sortByDistance(const Individual &a, const Individual &b);

vector<Individual> clone(vector<Individual> population, int **dm);

vector<int> siaAlgorithm(int populationSize, int cloneN, const vector<int> &solution, int **dm, clock_t begin);

#endif //TSP_FINAL_CLONALG_H
