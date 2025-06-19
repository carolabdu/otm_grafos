#include "ils.hpp"
#include <vector>
#include "utils.hpp"
#include <memory>
#include <random>
#include <algorithm>

KPFSolution PerturbSolution(const KPFSolution& solution, int strength) {
    // Clone via pointer to avoid deleted assignment
    auto perturbed_ptr = std::make_unique<KPFSolution>(solution.clone());
    int n = static_cast<int>(perturbed_ptr->x().size());

    // Conta quantos itens estão selecionados
    int selected_count = 0;
    for (int v : perturbed_ptr->x()) selected_count += v;

    // Inicializa gerador aleatório
    random_device rd;
    mt19937 gen(rd());

    // Decide aleatoriamente quantos remover (máximo strength e selected_count)
    std::uniform_int_distribution<int> remove_dist(0, std::min(strength, selected_count));
    int num_remove = remove_dist(gen);
    int num_add = strength - num_remove;

    // Separa índices selecionados e não selecionados
    std::vector<int> selected_indices;
    std::vector<int> not_selected_indices;
    for (int i = 0; i < n; ++i) {
        if (perturbed_ptr->x()[i] == 1) selected_indices.push_back(i);
        else not_selected_indices.push_back(i);
    }

    std::shuffle(selected_indices.begin(), selected_indices.end(), gen);
    std::shuffle(not_selected_indices.begin(), not_selected_indices.end(), gen);

    // Remove itens
    for (int i = 0; i < num_remove && i < (int)selected_indices.size(); ++i) {
        perturbed_ptr->toggleItem(selected_indices[i]);
    }

    // Adiciona itens
    for (int i = 0; i < num_add && i < (int)not_selected_indices.size(); ++i) {
        perturbed_ptr->toggleItem(not_selected_indices[i]);
    }

    return *perturbed_ptr; // retorna unique_ptr
}


KPFSolution ILS(const KPFSProblem& problem, int max_iterations, int perturbation_strength, float alpha, int relaxation){

    random_device rd;
    mt19937 gen(rd()); 
    KPFSolution initial = ConstructiveAlgorithm(problem, alpha);
    auto current_ptr = make_unique<KPFSolution>(BestImprovement(initial));
    float current_value = current_ptr->objectiveValue();
    int iterations_no_improvement = 0;
    auto best_ptr = make_unique<KPFSolution>(*current_ptr);
    float best_value = current_value;

    for (int i = 0; i < max_iterations; ++i){
        KPFSolution perturbed = PerturbSolution(*best_ptr, perturbation_strength);
        KPFSolution improved = BestImprovement(perturbed);

        if (improved.objectiveValue() > best_value) {
            best_ptr = make_unique<KPFSolution>(move(improved));
            best_value = improved.objectiveValue();
        }
        else{
            iterations_no_improvement +=1;
        }

        current_ptr = make_unique<KPFSolution>(*best_ptr);
        current_value = best_value;

        if(iterations_no_improvement >= relaxation) break;
    }

    // Return the best solution found
    return *best_ptr;
}
