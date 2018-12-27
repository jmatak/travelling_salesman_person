#ifndef TSP_FINAL_UTILITY_H
#define TSP_FINAL_UTILITY_H

#include <fstream>
#include "vector"
#include "string"

using namespace std;

string logSolution(const vector<int> &solution, int distance, double runningTime, const string &instance,
                   const string &algorithm);

#endif
