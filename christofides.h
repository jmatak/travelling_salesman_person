#ifndef TSP_FINAL_CHRISTOFIDES_H
#define TSP_FINAL_CHRISTOFIDES_H

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

using namespace std;

int minimumIndex(const int mins[], const int used[], int size);

void minimumSpanningTree(vector<pair<int, int>> &mst, int **dist, int size);

vector<int> greedyTourMatching(vector<int> graph, int **distanceMatrix);

void greedyMinWeightMatching(vector<pair<int, int>> &mst, int **distanceMatrix, int size);

vector<vector<int>> eulerCycles(vector<pair<int, int>> mst, int **distanceMatrix, int size);

int containsIndex(vector<int> v, int s);

void deleteDuplicates(vector<int> &tour);

vector<int> hamiltonianPath(vector<vector<int>> cycles, int **distanceMatrix, int size);

vector<int> christofidesAlgorithm(int **distanceMatrix, int size);

#endif //TSP_FINAL_CHRISTOFIDES_H
