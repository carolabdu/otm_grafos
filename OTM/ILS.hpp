#ifndef ILS_HPP
#define ILS_HPP

#include "classes_base.hpp"

KPFSolution PerturbSolution(const KPFSolution& solution, int strength = 1);
KPFSolution ILS(const KPFSProblem& problem, int max_iterations = 100, int perturbation_strength = 2, float alpha = 0.2);

#endif