#include "helpers.hpp"
#include <random>
#include "types.hpp"

std::random_device rd;
std::mt19937 rng(rd());

double default_probability_generator() {
    // Generate pseudorandom numbers between [0, 1); 10 bits of randomness.
    return std::generate_canonical<double, 10>(rng);
}

ProbabilityGenerator probability_generator = default_probability_generator;