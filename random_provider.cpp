
#include <random>

using namespace std;

// Initializer for random number with specific seed
std::random_device seed;
std::mt19937 rnd(seed());

/**
 * Getting random intiger from uniform distribution between lower and upper bounds provided as input.
 *
 * @param lowerBound Lower bound for random integer
 * @param upperBound Upper bound for random integer
 * @return Random integer between lower and upper bound
 */
int iRand(unsigned long lowerBound, unsigned long upperBound) {
    std::uniform_int_distribution<std::mt19937::result_type> distribution(lowerBound,
                                                                          upperBound); // distribution in range [1, 6]
    return static_cast<int>(distribution(rnd));
}

// Initializer for random numbers picked from uniform distribution bounded from 0 - 1
std::uniform_real_distribution<> uniform(0.0, 1.0);

/**
 * Function for getting random number from uniform distribution
 *
 * @return Random double from uniform distribution
 */
double fRand() {
    return uniform(rnd);
}