#ifndef GRASP_HPP
#define GRASP_HPP

#include "classes_base.hpp"

using namespace std;

KPFSolution GRASP(const KPFSProblem& problem, int maxIterations = 100, int relaxation = 5, float alpha = 0.8);

#endif