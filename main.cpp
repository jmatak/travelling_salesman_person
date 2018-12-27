#include <iostream>
#include <sstream>
#include <vector>
#include <iterator>
#include <math.h>
#include <random>
#include <fstream>
#include <cstring>
#include "graph.h"
#include "opt2local.h"
#include "christofides.h"
#include "sia.h"
#include "utility.h"
#include "simulated_annealing.h"
#include "ant_colony_optimization.h"
#include "branch_bound.h"

using namespace std;


/**
 * Reads input from /sample directory right next to binary file
 *
 * @param size Size of TSP instance
 * @param file File from which we read TSP instance
 * @return  Distance matrix from TSP tour
 */
int **readFileInput(int *size, const string &file) {
    string dir = "./samples/";
    string ext = ".tsp";
    ifstream inputFile(dir + file + ext);
    string oneLine;

    vector<Vertex> vertices;

    while (inputFile) {
        getline(inputFile, oneLine);
        istringstream coordinates(oneLine);

        vector<string> tokens{istream_iterator<string>{coordinates},
                              istream_iterator<string>{}};

        if (tokens.size() < 2) break;

        Vertex v = Vertex(stod(tokens[0]), stod(tokens[1]));
        vertices.push_back(v);
    }

    inputFile.close();

    *size = (int) vertices.size();
    int **distanceMatrix = new int *[*size];
    for (int i = 0; i < *size; ++i) {
        distanceMatrix[i] = new int[*size];

        for (int j = 0; j < i; ++j) {
            distanceMatrix[i][j] = distanceMatrix[j][i] =
                    calculateDistance(vertices[i], vertices[j]);
        }
    }
    return distanceMatrix;
}

/**
 * Reads input from standard IO stream. First reads number of cities followed by coordinates of each.
 *
 * @param size Size of TSP instance
 * @return  Distance matrix from TSP tour
 */
int **readInput(int *size) {
    cin >> *size;
    vector<Vertex> vertices;
    string oneLine;

    cin.ignore();
    for (int i = 0; i < *size; ++i) {
        std::getline(cin, oneLine);
        istringstream coordinates(oneLine);

        vector<string> tokens{istream_iterator<string>{coordinates},
                              istream_iterator<string>{}};

        Vertex v = Vertex(stod(tokens[0]), stod(tokens[1]), i);
        vertices.push_back(v);
    }

    int **distanceMatrix = new int *[*size];
    for (int i = 0; i < *size; ++i) {
        distanceMatrix[i] = new int[*size];

        for (int j = 0; j < i; ++j) {
            distanceMatrix[i][j] = distanceMatrix[j][i] =
                    calculateDistance(vertices[i], vertices[j]);
        }
    }
    return distanceMatrix;
}

/**
 * Encapsulation of Solution
 */
struct Solution {
    vector<int> solution;
    int distance;
    double duration;
    string instance;
    string algorithm;
};

/**
 * Runs specific algorithm, can be choosed from:
 *  CHRISTOFIDES    -   Christofides' algorithm
 *  SIA             -   Simple immunological algorithm on top of Christofides construction
 *  ACO             -   Ant colony optimization algorithm
 *  SA              -   Simulated annealing on top of Christofides
 *  BNB             -   Branch and Bound algorithm for small instances
 *
 * @param alg Code of algorithm
 * @param distanceMatrix Distance matrix
 * @param size Size of TSP tour
 * @return solution of specific algorithm
 */
Solution runSpecific(const string &alg, int **distanceMatrix, int size) {
    clock_t begin = clock();
    vector<int> solution;

    if (alg == "CHRISTOFIDES") {
        solution = christofidesAlgorithm(distanceMatrix, size);
        solution = opt2Algorithm(solution, distanceMatrix, size);
    } else if (alg == "SIA") {
        solution = christofidesAlgorithm(distanceMatrix, size);
        solution = opt2Algorithm(solution, distanceMatrix, size);
        solution = siaAlgorithm(9, 3, solution, distanceMatrix, begin);
    } else if (alg == "ACO") {
        solution = antColonySolver(distanceMatrix, size);
    } else if (alg == "SA") {
        solution = christofidesAlgorithm(distanceMatrix, size);
        solution = opt2Algorithm(solution, distanceMatrix, size);
        solution = simulatedAnnealing(solution, distanceMatrix, size);
    } else if (alg == "BNB") {
        solution = branchNBound(distanceMatrix, size);
    } else {
        cout << "Not valid ALGORITHM argument" << endl;
        exit(1);
    }
    Solution result;
    result.solution = solution;
    result.distance = calculateTourDistance(solution, distanceMatrix);
    result.duration = clock() - begin;
    return result;
}

/**
 * Script tunning main algorithm with SIA on top of Christofides
 *
 * @param distanceMatrix DIstance matrix
 * @param size Size of TSP tour
 * @return solution of named algorithm
 */
Solution runAlgorithm(int **distanceMatrix, int size) {
    clock_t begin = clock();
    vector<int> solution;

    solution = christofidesAlgorithm(distanceMatrix, size);
    solution = opt2Algorithm(solution, distanceMatrix, size);
    /* 9 is population size, 3 is clone size, -- aware of MAGIC CONSTANTS */
    solution = siaAlgorithm(9, 3, solution, distanceMatrix, begin);
    Solution result;
    result.solution = solution;
    result.distance = calculateTourDistance(solution, distanceMatrix);
    result.duration = clock() - begin;
    return result;
}

/**
 * Running on specific instance provided in /sample directory and outputs logging file as well as visualization
 * if python is included
 *
 * <program> <instance> <algorithm> v
 * Last one is optional
 *
 * @param argc
 * @param argv
 */
void fileRunner(int argc, char *argv[]) {
    int size;
    int **distanceMatrix = readFileInput(&size, argv[1]);
    if (size <= 3) {
        for (int i = 0; i < size; ++i) {
            cout << i << endl;
        }
        return;
    }
    string alg = argv[2];
    Solution s = runSpecific(alg, distanceMatrix, size);

    s.instance = argv[1];
    s.algorithm = argv[2];
    string file = logSolution(s.solution, s.distance, s.duration, s.instance, s.algorithm);

    if (argc == 4) {
        string command = "python3 ./visualize.py " + s.instance + " " + file;
        char cmnd[command.length() + 1];
        strcpy(cmnd, command.c_str());
        system(cmnd);
    }
    for (int i : s.solution) {
        cout << i << endl;
    }
}

/**
 * Runs specific algorithm with command line input
 *
 * run program like : <program> <alg>
 *
 * @param argv
 */
void commandSpecificRunner(char *argv[]) {
    int size;
    int **distanceMatrix = readInput(&size);
    if (size <= 3) {
        for (int i = 0; i < size; ++i) {
            cout << i << endl;
        }
        return;
    }
    string alg = argv[1];
    Solution s = runSpecific(alg, distanceMatrix, size);
    for (int i : s.solution) {
        cout << i << endl;
    }
}

/**
 * Running the program with command line input
 *
 * No input parameters
 */
void commandLineRunner() {
    int size;
    int **distanceMatrix = readInput(&size);
    if (size <= 3) {
        for (int i = 0; i < size; ++i) {
            cout << i << endl;
        }
        return;
    }
    Solution s = runAlgorithm(distanceMatrix, size);
    for (int i : s.solution) {
        cout << i << endl;
    }
}

/**
 * Starting fuction of a program
 */
int main(int argc, char *argv[]) {
    if (argc == 1) {
        commandLineRunner();
    } else if (argc == 2) {
        commandSpecificRunner(argv);
    } else if (argc == 4 || argc == 3) {
        fileRunner(argc, argv);
    } else {
        cout << "Invalid number of arguments";
        return 1;
    }
    return 0;
}