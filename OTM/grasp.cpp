#include <vector>
#include "grasp.hpp"
#include "utils.hpp"
#include <memory>
#include <random>

using namespace std;


KPFSolution GRASP(const KPFSProblem& problem, int nRandom, int maxIterations, int relaxation) {
    // Initialize best_ptr with an empty solution
    auto best_ptr = make_unique<KPFSolution>(problem);
    float best_value = best_ptr->objectiveValue();
    int iterations_no_improvement = 0;

    random_device rd;
    mt19937 gen(rd());
    auto candidates = RandomSolutions(problem, nRandom);
    for (int iter = 0; iter < maxIterations; ++iter) {

        KPFSolution current_solution = ConstructiveAlgorithm(candidates);
        // Busca Local
        auto improved_ptr = make_unique<KPFSolution>(BestImprovement(current_solution));
        float val = improved_ptr->objectiveValue();
        if (val > best_value) {
                best_value = val;
                best_ptr = move(improved_ptr);
            }
        else{
            iterations_no_improvement +=1;
        }
        if (iterations_no_improvement >= relaxation) break;
        }
    

    // Return the best solution (by copy constructing)
    return *best_ptr;
}
