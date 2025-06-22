#include <vector>
#include "grasp.hpp"
#include "utils.hpp"
#include <memory>
#include <random>
#include <iostream>

using namespace std;


KPFSolution GRASP(const KPFSProblem& problem, int maxIterations, int relaxation, float alpha) {

    auto best_ptr = make_unique<KPFSolution>(problem);
    float best_value = best_ptr->objectiveValue();
    
    int iterations_no_improvement = 0;
    int iter;
    for (iter = 0; iter < maxIterations; ++iter) {
        // Criação de um número aleatório para ser usado no algoritmo construtivo
        random_device rd;
        mt19937 gen(rd());
        KPFSolution current_solution = ConstructiveAlgorithm(problem, alpha, gen);
        // Busca Local
        auto improved_ptr = make_unique<KPFSolution>(BestImprovement(current_solution));
        float val = improved_ptr->objectiveValue();
        // Verificação se a solução atual é superior à melhor
        if (val > best_value) {
            best_value = val;
            best_ptr = move(improved_ptr);
            iterations_no_improvement = 0;
        } else {
            iterations_no_improvement +=1;
        }
        // Ao alcançar um número de iterações sem melhoria igual ao valor de relaxation
        // interrompemos a execução
        if (iterations_no_improvement >= relaxation) break;
    }
    
    return *best_ptr;
}
