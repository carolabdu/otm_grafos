#include <vector>
#include "grasp.hpp"
#include "utils.hpp"
#include <memory>
#include <random>
#include <iostream>

using namespace std;


KPFSolution GRASP(const KPFSProblem& problem, int maxIterations, int relaxation, float alpha) {
    // Initialize best_ptr with an empty solution
    auto best_ptr = make_unique<KPFSolution>(problem);
    float best_value = best_ptr->objectiveValue();
    int iterations_no_improvement = 0;

    random_device rd;
    mt19937 gen(rd());
    int iter;
    for (iter = 0; iter < maxIterations; ++iter) {

        KPFSolution current_solution = ConstructiveAlgorithm(problem, alpha, gen);
        // Busca Local
        auto improved_ptr = make_unique<KPFSolution>(BestImprovement(current_solution));
        float val = improved_ptr->objectiveValue();
        if (val > best_value) {
                best_value = val;
                best_ptr = move(improved_ptr);
                iterations_no_improvement = 0;
            }
        else{
            iterations_no_improvement +=1;
        }
        if (iterations_no_improvement >= relaxation) break;
        }
    
    // Return the best solution (by copy constructing)
    return *best_ptr;
}
