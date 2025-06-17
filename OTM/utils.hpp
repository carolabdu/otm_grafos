#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include "classes_base.hpp"

using namespace std;
KPFSolution ConstructiveAlgorithm(const KPFSProblem& problem, float alpha = 0.2);
KPFSolution BestImprovement(const KPFSolution& original_solution);

#endif