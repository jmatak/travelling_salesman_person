#include <fstream>
#include "utility.h"
#include "vector"
#include "string"

using namespace std;

/**
 * Get current time
 *
 * @return current time
 */
string getTime() {
    time_t rawtime;
    struct tm *timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d_%m_%Y_%H:%M:%S", timeinfo);
    std::string str(buffer);
    return str;
}

/**
 * Prints solution in for of logs:
 *  <solution>
 *  <distance>
 *  <running_time>
 *  <instance>
 *  <algorithm>
 *
 * @param solution Solution as vector of indices
 * @param distance Distance of route
 * @param runningTime Running time in algorithm
 * @param instance Instance of TSP problem
 * @param algorithm Algorithm used to solve TSP
 * @return name of log file
 */
string logSolution(const vector<int> &solution, int distance, double runningTime, const string &instance,
                   const string &algorithm) {
    ofstream f;
    string directory = "./logs/";
    string ext = ".tsp";
    auto time = getTime();
    f.open(directory + algorithm + "_" + instance + "_" + time + ext);
    for (int i = 0; i < solution.size(); ++i) {
        if (i != solution.size() - 1) {
            f << solution[i] << ",";
            continue;
        }
        f << solution[i] << endl;
    }
    f << distance << endl;
    f << runningTime / CLOCKS_PER_SEC << endl;
    f << instance << endl;
    f << algorithm << endl;
    f.close();
    return algorithm + "_" + instance + "_" + time;
}