#ifndef TABU_HPP
#define TABU_HPP

#include "classes_base.hpp"


KPFSolution TabuSearch(const KPFSProblem& problem,
                       int max_iter = 100,
                       int max_tabu_size = 10);

#endif