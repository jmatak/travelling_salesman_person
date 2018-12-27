#ifndef TSP_FINAL_BRANCH_BOUND_H
#define TSP_FINAL_BRANCH_BOUND_H

#include <random>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <math.h>
#include <random>
#include <algorithm>
#include <climits>
#include <list>
#include <vector>

using namespace std;

void rowInfinity(int **matrix, int position, int size);
void columnInfinity(int **matrix, int position, int size);
int **copyMatrix(int **old, int size);
int **createCostMatrix(int **distanceMatrix, int size);
int **reduceMatrix(int **matrix, int size, int *cost);
vector<int> branchNBound(int **distanceMatrix, int size);

class CityNode {
public:
    CityNode *parent = NULL;
    vector<CityNode *> children;
    vector<int> notUsed;
    int **matrix;
    int cost;
    int size;
    int position;
    int depth;

    CityNode(CityNode *parent, vector<int> notUsed, int **matrix, int cost, int size, int position) : notUsed(notUsed),
                                                                                                      matrix(matrix),
                                                                                                      cost(cost),
                                                                                                      size(size),
                                                                                                      position(
                                                                                                              position) {
        this->parent = parent;
        this->depth = this->parent->depth + 1;
    };

    CityNode(vector<int> notUsed, int **matrix, int cost, int size, int position) {
        this->notUsed = notUsed;
        this->matrix = matrix;
        this->cost = cost;
        this->size = size;
        this->position = position;
        this->depth = 0;
    };

    bool isLeaf();

    void branch();

    void getCost();

    void rowColumnInfinity();

    CityNode *iterate();

    CityNode *getBestChild();

    void setParentInfinity();

    vector<int> reconstructPath();
};

void boundRemoval(CityNode *solution);
CityNode* pickNext();


#endif //TSP_FINAL_ANT_COLONY_OPTIMIZATION_H
