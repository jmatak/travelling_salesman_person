#include <vector>
#include <algorithm>
#include "iostream"
#include <ctime>
#include "opt2local.h"
#include "graph.h"
#include "random_provider.h"

using namespace std;

typedef pair<vector<int>, int> Individual;

/**
 * Function for creating neighbor in SIA algorithm, neighbor is defined as cycle in which two random vertices are
 * swapped and Opt2 local optimization is performed.
 *
 * @param solution Current cycle
 * @param dm Distance matrix
 * @return Neighbor created by switching two vertices and doing Opt2
 */
Individual createNeighbor(vector<int> solution, int **dm, int noSwitches) {
    vector<int> v(solution.size());
    vector<int> switched;
    for (int i = 0; i < noSwitches; i++) {
        int index = iRand(0, solution.size() - 1);
        auto it = find(switched.begin(), switched.end(), index);
        if (it != switched.end()) {
            i--;
        } else {
            switched.push_back(index);
        }
    }
    for (int i = 0; i < solution.size(); ++i) {
        bool found = false;
        for (int j = 0; j < switched.size(); j++) {
            if (i == switched[j]) {
                v[i] = solution[switched[(j + 1) % switched.size()]];
                found = true;
                break;
            }
        }
        if (!found) {
            v[i] = solution[i];
        }
    }
    v = opt2Algorithm(v, dm, (int) v.size());
    return make_pair(v, calculateTourDistance(v, dm));
}

/**
 * Function that takes first solution and creates neighborhood with size of population.
 *
 * @param solution First solution, obtained by some construction algorithm
 * @param popSize Size of population in SIA
 * @param dm Distance matrix
 * @return Vector representing current population
 */
vector<Individual> initializeSolutions(const vector<int> &solution, int popSize, int **dm) {
    auto population = vector<Individual>();
    for (int i = 0; i < popSize; ++i) {
        population.push_back(createNeighbor(solution, dm, 2));
    }
    return population;
}

/**
 * Sorting two individuals by distance
 *
 * @param a Individual A
 * @param b Individual B
 * @return True if A is smaller than  B
 */
bool sortByDistance(const Individual &a, const Individual &b) {
    return (a.second < b.second);
}

/**
 * Creating new population by cloning the best indivuduals and performing mutation (finding neighbor) that is not
 * differing by much from the provided solution.
 *
 * @param population Current population in algorithm
 * @param dm  Distance matrix
 * @return Vector of cloned population
 */
vector<Individual> clone(vector<Individual> population, int mBest, int **dm) {
    vector<Individual> newPopulation = vector<Individual>();
    newPopulation.push_back(population[0]);
    for (int i = 0; i < mBest; ++i) {
        int switches = 2;
        for (int j = 0, n = (int) (population.size() / mBest); j < n; ++j) {
            newPopulation.push_back(createNeighbor(population[i].first, dm, switches));
        }
    }
    return newPopulation;
}

/**
 * Core of SIA, firstly population is created to contain DNA of first solution, then it is sorted by best solution,
 * and first n best are cloned and mutated to create next population. This algorithm is elitistic since best individual
 * is always preserved. Algorithm runs till 2s deadline is reached.
 *
 * @param begin Algorithm's starting time
 * @param solution Solution that carries DNA of first population
 * @param dm Distance matrix
 * @return Best tour obtained in algorithm
 */
vector<int> siaAlgorithm(int populationSize, int cloneN, const vector<int> &solution, int **dm, clock_t begin) {
    auto population = initializeSolutions(solution, populationSize, dm);
    sort(population.begin(), population.end(), sortByDistance);
    int iter = 0;
    while (true) {
        if ((double(clock() - begin) / CLOCKS_PER_SEC) >= 1.9) break;
        population = clone(population, cloneN, dm);
        sort(population.begin(), population.end(), sortByDistance);
        population.resize(static_cast<unsigned long>(populationSize));
        iter++;
    }
    return population[0].first;
}
