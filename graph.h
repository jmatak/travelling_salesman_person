#ifndef TSP_FINAL_GRAPH_H
#define TSP_FINAL_GRAPH_H

#include <vector>
#include <math.h>

using namespace std;

// Class representing vertex in TSP
class Vertex {
public:
    double x;
    double y;
    int position;

    Vertex(double x, double y) : x(x), y(y) {}

    Vertex(double x, double y, int position) : x(x), y(y), position(position) {}

};

vector<int> getNeighbours(int i);

vector<int> findNeighbors(int index, int noCandidates, int **distanceMatrix, int size);

void initializeCandidatesList(int k, int **distanceMatrix, int size);

int calculateDistance(Vertex v1, Vertex v2);

int calculateTourDistance(vector<int> solution, int **distanceMatrix);

#endif