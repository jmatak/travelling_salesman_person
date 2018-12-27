#include "simulated_annealing.h"
#include "random_provider.h"
#include "opt2local.h"
#include "graph.h"

/**
 * Class that can create neighbor of a solution, by switching cities
 */
vector<int> Neighborhood::randomNeighbor(const vector<int> &solution, int **distanceMatrix) {
    unsigned long size = solution.size();
    int n1 = iRand(0, size - 1);
    int n2 = iRand(0, size - 1);
    return createNeighbor(solution, n1, n2, distanceMatrix);
};

/**
 * Random neighbor by switching 2 cities at indexes n1 and n2
 *
 * @param solution
 * @param n1 First random index
 * @param n2 Second random index
 * @param distanceMatrix Matrix defining distances in graph
 * @return
 */
vector<int> Neighborhood::createNeighbor(vector<int> solution, int n1, int n2, int **distanceMatrix) {
    unsigned long size = solution.size();
    vector<int> v;
    for (int i = 0; i < size; i++) {
        if (i == n1) {
            v.push_back(solution[n2]);
        } else if (i == n2) {
            v.push_back(solution[n1]);
        } else {
            v.push_back(solution[i]);
        }
    }
    return v;
}

/**
 * Lowers the temperature and returns it
 * @return Next temperature
 */
double GeometricTempSchedule::getNextTemperature() {

    double returnValue = tCurrent;
    if (tCurrent < tEnding) {
        return tEnding;
    }
    tCurrent = tCurrent * alpha;
    return returnValue;
};

/**
 * Runs through 2 loops, the outer lowers the temperature while the inner creates neighbors.
 * If the solution has better fitness (lower cost), it becomes the current solution, but if it doesn't,
 * there is still a probability based on the temperature that it will be chosen.
 */
void SimulatedAnnealing::run() {
    vector<int> solution = startWith;
    vector<int> globalBest = startWith;

    int globalFitness = calculateTourDistance(globalBest, distanceMatrix);

    for (int i = 0, outerLoop = tempSchedule.outerLimit; i < outerLoop; i++) {
        //Lowers the temperature
        double temperature = tempSchedule.getNextTemperature();

        for (int j = 0, innerLoop = tempSchedule.innerLimit; j < innerLoop; j++) {

            int n1 = iRand(0, (unsigned long) size - 1);
            int n2 = iRand(0, (unsigned long) size - 1);

            //Assigns the neighbro
            vector<int> neighbor = neighborhood.createNeighbor(solution, n1, n2, distanceMatrix);


            int solutionFitness = calculateTourDistance(solution, distanceMatrix);
            int neighborFitness = calculateTourDistance(neighbor, distanceMatrix);
            //int neighborFitness = calculateNeighborFitness(solution, neighbor, n1, n2, solutionFitness, (int)size, distanceMatrix);

            int deltaEnergy = neighborFitness - solutionFitness;

            //Assigns the current solution if better or with certain probability
            if (deltaEnergy <= 0) {
                solution = neighbor;
                solutionFitness = neighborFitness;

            } else if (fRand() <= exp(-deltaEnergy / temperature)) {
                solution = neighbor;
                solutionFitness = neighborFitness;
            }

            if (solutionFitness <= globalFitness) {
                globalBest = solution;
                globalFitness = solutionFitness;
            }
        }

        solution = opt2Algorithm(solution, distanceMatrix, size);
        int solutionFitness = calculateTourDistance(solution, distanceMatrix);

        //Update global best if you found it
        if (globalFitness >= solutionFitness) {
            globalBest = solution;
            globalFitness = calculateTourDistance(solution, distanceMatrix);
        }
    }


    best = globalBest;
}

/**
 * Starts the simmulated annealing algorithm
 *
 * @param startSolution Starting solution in algorithm
 * @param distanceMatrix Distance matrix
 * @param size Size of tsp instance
 * @return
 */
vector<int> simulatedAnnealing(vector<int> &startSolution, int **distanceMatrix, int size) {
    //alpha, initial, inner, outer
    GeometricTempSchedule tempSchedule(0.98, 100, 0.001, 100, 500);
    Neighborhood neighborhood;

    SimulatedAnnealing s(startSolution, tempSchedule, neighborhood, distanceMatrix, size);
    s.run();

    vector<int> best = s.best;

    return best;
}

