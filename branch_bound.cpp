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
#include "graph.h"
#include "branch_bound.h"
#include "random_provider.h"

using namespace std;

/**
 * Fills the vector from 0 to size - 1 with the same values
 * @param size  size of the vector
 * @return  filled vector
 */
vector<int> fillVector(int size) {
    vector<int> v;
    for (int i = 0; i < size; i++) {
        v.push_back(i);
    }

    return v;
}

/**
 * Sets certain row values to infinity.
 * @param matrix Matrix given for adding infinity to a certain row
 * @param position position of the row
 * @param size size of the quadratic matrix
 */
void rowInfinity(int **matrix, int position, int size) {
    for (int j = 0; j < size; j++) {
        matrix[position][j] = INT_MAX;
    }
}

/**
 * Sets a certain column values to infinity
 * @param matrix  Matrix given for adding infinity to a certain column
 * @param position Position of the column
 * @param size Size of the quadratic matrix
 */
void columnInfinity(int **matrix, int position, int size) {
    for (int j = 0; j < size; j++) {
        matrix[j][position] = INT_MAX;
    }
}

/**
 * Copies one vector to another
 * @param path vector to be copied
 * @return copied vector
 */
vector<int> copyVector(vector<int> path) {
    vector<int> v;
    for (unsigned long i = 0, size = path.size(); i < size; i++) {
        v.push_back(path[i]);
    }

    return v;
}

/**
 * Copies one matrix to another
 * @param old Matrix to be copied
 * @param size Size of the quadratic matrix
 * @return copied matrix
 */
int **copyMatrix(int **old, int size) {
    int **matrix = new int *[size];
    for (int i = 0; i < size; ++i) {
        matrix[i] = new int[size];

        for (int j = 0; j < size; ++j) {
            matrix[i][j] = old[i][j];
        }
    }
    return matrix;
}

/**
 * Creates a cost matrix by setting the diagonal values to infinity.
 * @param distanceMatrix Distance matrix between cities
 * @param size Size of the distance matrix (quadratic)
 * @return Cost matrix with distances between cities that are not paths to itself
 */
int **createCostMatrix(int **distanceMatrix, int size) {
    int **costMatrix = new int *[size];
    for (int i = 0; i < size; ++i) {
        costMatrix[i] = new int[size];

        for (int j = 0; j <= i; ++j) {
            if (j == i) {
                costMatrix[i][j] = INT_MAX;
            } else {
                costMatrix[i][j] = costMatrix[j][i] = distanceMatrix[i][j];
            }
        }
    }
    return costMatrix;
}

/**
 * Reduces a matrix and updates the cost of reduction.
 * @param matrix  Matrix that is reduced
 * @param size Size of the matrix
 * @param cost Cost of reduction of matrix
 * @return Reduced matrix
 */
int **reduceMatrix(int **matrix, int size, int *cost) {
    for (int i = 0; i < size; i++) {
        int min = -1;
        for (int j = 0; j < size; j++) {
            if (i == j) continue;
            min = (min == -1 || min > matrix[i][j]) ? matrix[i][j] : min;
        }

        if (min != INT_MAX && min != 0) {
            *cost += min;
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    matrix[i][j] = INT_MAX;
                    continue;
                }

                matrix[i][j] = (matrix[i][j] == INT_MAX) ? matrix[i][j] : matrix[i][j] - min;
            }
        }
    }

    for (int i = 0; i < size; i++) {
        int min = -1;
        for (int j = 0; j < size; j++) {
            if (i == j) continue;
            min = (min == -1 || min > matrix[j][i]) ? matrix[j][i] : min;
        }
        if (min != INT_MAX && min != 0) {
            *cost += min;
            for (int j = 0; j < size; j++) {
                if (i == j) {
                    continue;
                }
                matrix[j][i] = (matrix[j][i] == INT_MAX) ? matrix[j][i] : matrix[j][i] - min;
            }
        }
    }

    return matrix;
}

/** Upper bound of cost. All nodes will be deleted that have a higher cost than this.*/
int upper = INT_MAX;

/** Leaf nodes, possible expansion nodes for finding a solution.*/
list<CityNode*> leafs;


/**
 * Check if the node does not have any children.
 * @return true if the node is leaf
 */
bool CityNode::isLeaf() {
    return children.empty();
};

/**
 * Branches current node into children, and puts them into the leafs array, therefore deleting it's place in the leafs array since it branched
 * and has no children anymore.
 */
void CityNode::branch() {

    if (notUsed.empty()) return;

    for (int i = 0; i < notUsed.size(); i++) {
        vector<int> v = copyVector(notUsed);
        int pos = v[i];
        v.erase(v.begin() + i);
        shuffle(v.begin(), v.end(), std::mt19937(std::random_device()()));
        CityNode *node = new CityNode(this, v, copyMatrix(matrix, size), 0, size, pos);
        node->getCost();

        if (node->cost < upper) {
            leafs.push_front(node);
            children.push_back(node);
        }
    }

    leafs.remove(this);
};

/**
 * Sets the row and column of the node and parent to infinity, as well as all the costs from children to parent.
 * Reduces the matrix and calculates the total cost of the node by adding parent cost, reduction cost and the cost from
 * going from parent node to child node.
 */
void CityNode::getCost() {
    rowColumnInfinity();
    setParentInfinity();
    int reduceCost = 0;
    matrix = reduceMatrix(matrix, size, &reduceCost);
    cost = reduceCost;
    if (parent != NULL) {
        cost += parent->matrix[parent->position][position] + parent->cost;
    }
}

/**
 * Sets parent row to infinity and column of the node to infinity
 */
void CityNode::rowColumnInfinity() {
    rowInfinity(this->matrix, this->parent->position, size);
    columnInfinity(this->matrix, this->position, size);
}

/**
 * Selects next solution candidate for changing the upper bound and finding the solution to the problem.
 * @return Solution found after iteration
 */
CityNode* CityNode::iterate() {
    if (notUsed.empty()) return this;

    if (children.empty()) {
        branch();
    }

    if (!children.empty() && children[0]->notUsed.empty()) {
        return getBestChild();
    }
    return pickNext()->iterate();
}

/**
 * Gets the best child with lowest cost from a node
 * @return lowest cost child
 */
CityNode* CityNode::getBestChild() {
    CityNode* best = children[0];
    for (int i = 1, s = children.size(); i < s; i++) {
        if (children[i]->cost < best->cost) {
            best = children[i];
        }
    }
    return best;
}

/**
 * Sets the positions from parents to node to infinity.
 */
void CityNode::setParentInfinity() {
    CityNode* temp = parent;
    while (temp != NULL) {
        matrix[position][temp->position] = INT_MAX;
        temp = temp->parent;
    }
}

/**
 * Reconstructs the TSP tour from a node by going backtracking.
 * @return TSP tour
 */
vector<int> CityNode::reconstructPath() {
    vector<int> path;
    CityNode *temp = this;
    while (temp != NULL) {
        path.push_back(temp->position);
        temp = temp->parent;
    }
    return path;
}

/**
 * Removes all the solutions that have a higher bound than the upper variable (global).
 * @param solution The solution not to be erased.
 */
void boundRemoval(CityNode *solution) {
    auto it = leafs.begin();
    while (it != leafs.end()) {
        // Remove elements while iterating
        if ((*it) != solution && (*it)->cost >= upper) {
            it = leafs.erase(it);
        } else
            it++;
    }
}

/**
 * Picks next possible candidate for branching into a solution, from the leafs array.
 * @return Next city node candidate to iterate and branch to.
 */
CityNode* pickNext() {
    CityNode *solution = NULL;
    auto it = leafs.begin();

    while (it != leafs.end()) {
        if (solution == NULL) {
            if (!(*it)->notUsed.empty()) {
                solution = *it;
            }
        } else if (solution->cost > (*it)->cost && !(*it)->notUsed.empty()) {
            solution = *it;
        }
        it++;
    }

    if (solution != NULL) {
        return solution;
    }
    return leafs.front();
}

/**
 * Starts the branch and bound algorithm, by beginning with a random node, branching itself until it finds a solution. The cost of
 * solution updates the upper bound and every node that had a higher cost than the upper bound is therefore deleted. Process is repeated until
 * the solution with the optimal cost is selected
 * @param distanceMatrix Distance matrix between cities.
 * @param size Size of the distance matrix
 * @return Optimal tsp tour.
 */
vector<int> branchNBound(int **distanceMatrix, int size) {
    int cost = 0;
    int **costMatrix = createCostMatrix(distanceMatrix, size);
    int **reducedMatrix = reduceMatrix(costMatrix, size, &cost);

    vector<int> notUsed = fillVector(size);
    int number = notUsed[0];
    notUsed.erase(notUsed.begin());
    shuffle(notUsed.begin(), notUsed.end(), std::mt19937(std::random_device()()));
    CityNode root = CityNode(notUsed, reducedMatrix, cost, size, number);
    leafs.push_front(&root);

    CityNode *node = &root;
    CityNode *solution = NULL;
    while (true) {
        solution = node->iterate();
        int bound = solution->cost;
        if (bound > upper) {
            leafs.remove(solution);
        } else {
            upper = bound;
            boundRemoval(solution);
        }
        if (leafs.size() == 1 && (*leafs.begin())->notUsed.empty()) {
            break;
        }
        node = pickNext();
        if (node->notUsed.empty()) {
            solution = node;
            break;
        }
    }


    return solution->reconstructPath();
}