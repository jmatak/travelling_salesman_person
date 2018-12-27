#ifndef TSP_FINAL_SIMULATED_ANNEALING_H
#define TSP_FINAL_SIMULATED_ANNEALING_H

#include "vector"

using namespace std;

/**
 * Class that can create neighbor of a solution, by switching cities
 */
class Neighborhood {
public:
    vector<int> randomNeighbor(const vector<int> &solution, int **distanceMatrix);

    vector<int> createNeighbor(vector<int> solution, int n1, int n2, int **distanceMatrix);
};

/**
 * Schedule that controls the flow of temperature, lowers it every iteration
 */
class GeometricTempSchedule {
public:
    /** The factor that lowers the temperature every time. */
    double alpha;
    double tInitial;
    double tEnding;
    double tCurrent;
    int innerLimit;
    int outerLimit;

    GeometricTempSchedule(double alpha, double tInitial, double tEnding, int innerLimit,
                          int outerLimit) : alpha(alpha), tInitial(tInitial), tEnding(tEnding),
                                            innerLimit(innerLimit), outerLimit(outerLimit) {
        tCurrent = tInitial;
    };

    double getNextTemperature();
};

/**
 * Core of the algorithm
 */
class SimulatedAnnealing {
public:
    /** Starting solution.*/
    vector<int> startWith;

    /** Best found solution during the algorithm. */
    vector<int> best;

    /** Schedule for temperature.*/
    GeometricTempSchedule tempSchedule;

    /**Neighborhood for getting neighbor solutions.*/
    Neighborhood neighborhood;
    int size;
    int **distanceMatrix;

    SimulatedAnnealing(vector<int> &startWith,
                       GeometricTempSchedule tempSchedule,
                       Neighborhood neighborhood,
                       int **distanceMatrix,
                       int size) : startWith(startWith), tempSchedule(tempSchedule), neighborhood(neighborhood),
                                   distanceMatrix(distanceMatrix), size(size) {};

    void run();
};

vector<int> simulatedAnnealing(vector<int> &startSolution, int **distanceMatrix, int size);

#endif
