#include "ils.hpp"
#include <vector>
#include "utils.hpp"
#include <memory>
#include <random>
#include <algorithm>

KPFSolution PerturbSolution(const KPFSolution& solution, int strength) {

    auto perturbed_ptr = std::make_unique<KPFSolution>(solution.clone());
    const auto& bitset_x = perturbed_ptr->x();
    int n = MAX_ITEMS; 

    // List of indices
    std::vector<int> selected_indices;
    std::vector<int> not_selected_indices;

    // Coleta os indices baseado no estado bitset
    for (int i = 0; i < n; ++i) {
        if (i >= (int)perturbed_ptr->problem().items().size()) break;
        if (bitset_x.test(i)) selected_indices.push_back(i);
        else not_selected_indices.push_back(i);
    }

    // Geração de número aleatório
    std::random_device rd;
    std::mt19937 gen(rd());

    // Escolhe aleatoriamente uma quantidade de itens para remover <= min(strength, n)
    std::uniform_int_distribution<int> remove_dist(0, std::min(strength, (int)selected_indices.size()));
    int num_remove = remove_dist(gen);
    int num_add = strength - num_remove;

    std::shuffle(selected_indices.begin(), selected_indices.end(), gen);
    std::shuffle(not_selected_indices.begin(), not_selected_indices.end(), gen);

    // Remove itens
    for (int i = 0; i < num_remove && i < (int)selected_indices.size(); ++i) {
        perturbed_ptr->removeItem(selected_indices[i]);
    }

    // Adiciona itens
    for (int i = 0; i < num_add && i < (int)not_selected_indices.size(); ++i) {
        perturbed_ptr->addItem(not_selected_indices[i]);
    }

    return *perturbed_ptr;
}


KPFSolution ILS(const KPFSProblem& problem, int max_iterations, int perturbation_strength, float alpha, int relaxation){

    // Criação de número aleatório para o construtivo
    random_device rd;
    mt19937 gen(rd()); 
    // Uso do algoritmo construtivo para encontrar uma solução inicial
    KPFSolution initial = ConstructiveAlgorithm(problem, alpha, gen);
    // Busca Local
    auto current_ptr = make_unique<KPFSolution>(BestImprovement(initial));
    float current_value = current_ptr->objectiveValue();
    int iterations_no_improvement = 0;
    auto best_ptr = make_unique<KPFSolution>(*current_ptr);
    float best_value = current_value;

    for (int i = 0; i < max_iterations; ++i){

        // Perturbação da solução
        KPFSolution perturbed = PerturbSolution(*best_ptr, perturbation_strength);
        // Busca Local após a perturbação
        KPFSolution improved = BestImprovement(perturbed);

        // Critério de aceitação (se há melhoria na função objetivo)
        if (improved.objectiveValue() > best_value) {
            best_ptr = make_unique<KPFSolution>(move(improved));
            best_value = improved.objectiveValue();
            iterations_no_improvement = 0;
        }
        else{
            iterations_no_improvement +=1;
        }

        current_ptr = make_unique<KPFSolution>(*best_ptr);
        current_value = best_value;
        // Ao alcançar um número de iterações sem melhoria igual ao valor de relaxation
        // interrompemos a execução
        if (iterations_no_improvement >= relaxation) break;
    }

    return *best_ptr;
}



