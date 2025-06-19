#ifndef SIMULATED_ANNEALING_HPP
#define SIMULATED_ANNEALING_HPP

#include <random>
#include "classes_base.hpp"


KPFSolution Get_Random_Neighbour(const KPFSolution& solution, std::mt19937& gen);
KPFSolution Simulated_Annealing(KPFSProblem problem, int ASmax = 100, float T_min = 1e-3, float T_init = 100.0, float alpha = 0.8, int relaxation = 15);

#endif