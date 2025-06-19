#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include "classes_base.hpp"
#include <random>

using namespace std;
KPFSolution ConstructiveAlgorithm(const KPFSProblem& problem, float alpha, mt19937& rng);
KPFSolution BestImprovement(const KPFSolution& original_solution);

#endif