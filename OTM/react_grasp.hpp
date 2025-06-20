#ifndef REACT_GRASP_HPP
#define REACT_GRASP_HPP

#include <vector>
#include "classes_base.hpp"

using namespace std;

KPFSolution React_GRASP(const KPFSProblem& problem, vector<float> alpha_list, int blockIter, int maxIterations = 100, int relaxation = 15);

#endif