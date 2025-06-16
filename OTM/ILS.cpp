#include "ILS.hpp"
#include <vector>
#include "utils.hpp"
#include <memory>
#include <random>
#include <algorithm>

KPFSolution PerturbSolution(const KPFSolution& solution, int strength) {
    // Clone via pointer to avoid deleted assignment
    auto perturbed_ptr = std::make_unique<KPFSolution>(solution.clone());
    int n = static_cast<int>(perturbed_ptr->x().size());

    // Sample 'strength' distinct indices
    vector<int> indices(n);
    iota(indices.begin(), indices.end(), 0);
    random_device rd;
    mt19937 gen(rd());
    shuffle(indices.begin(), indices.end(), gen);

    for (int i = 0; i < strength && i < n; ++i) {
        perturbed_ptr->toggleItem(indices[i]);
    }

    return *perturbed_ptr;  // returns by copy (uses copy ctor)
}


KPFSolution ILS(const KPFSProblem& problem, int max_iterations, int perturbation_strength, float alpha){

    vector<KPFSolution> initial_solutions = RandomSolutions(problem, 10);
    KPFSolution current_solution = ConstructiveAlgorithm(initial_solutions, alpha);
    auto current_ptr = make_unique<KPFSolution>(BestImprovement(current_solution));
    float current_value = current_ptr->objectiveValue();

    // 3) Initialize best = current
    auto best_ptr = make_unique<KPFSolution>(*current_ptr);
    float best_value = current_value;

    for (int i = 0; i < max_iterations; ++i){
        KPFSolution perturbed = PerturbSolution(*best_ptr, perturbation_strength);
        KPFSolution improved = BestImprovement(perturbed);

        if (improved.objectiveValue() > best_value) {
            best_ptr = make_unique<KPFSolution>(move(improved));
            best_value = improved.objectiveValue();
        }

        // d) For next iteration, set current_ptr = improved (always keeps climbing)
        current_ptr = make_unique<KPFSolution>(*best_ptr);
        current_value = best_value;
    }

    // Return the best solution found
    return *best_ptr;
}