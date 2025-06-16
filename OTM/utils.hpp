#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include "classes_base.hpp"

using namespace std;
KPFSolution ConstructiveAlgorithm(vector<KPFSolution> solutions, float alpha = 0.2);
KPFSolution BestImprovement(const KPFSolution& original_solution);
vector<KPFSolution> RandomSolutions(const KPFSProblem& problem, int size = 5);

#endif