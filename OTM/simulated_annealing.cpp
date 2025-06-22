#include "simulated_annealing.hpp"
#include <vector>
#include "utils.hpp"
#include <memory>
#include <random>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>


KPFSolution Get_Random_Neighbour(const KPFSolution& solution, std::mt19937& gen) {

    KPFSolution neighbour = solution.clone();
    const auto& problem = solution.problem();
    size_t num_items = problem.items().size();

    // Escolha um índice aleatório válido
    std::uniform_int_distribution<> distrib(0, num_items - 1);
    int index = distrib(gen);
    // Inverte o estado de um item, conseguindo, assim, um vizinho
    neighbour.toggleItem(index);
    return neighbour;
}

KPFSolution Simulated_Annealing(KPFSProblem problem, int ASmax, float T_min, float T_init, float alpha, int relaxation){
    
    // Geração de um número aleatório
    random_device rd;
    mt19937 gen(rd());
    
    uniform_real_distribution<float> uni01(0.0f, 1.0f);

    // Uso de algoritmo construtivo para criar uma solução
    KPFSolution constructed = ConstructiveAlgorithm(problem, 1, gen);

    auto s_star_ptr    = make_unique<KPFSolution>(constructed.clone());
    auto s_current_ptr = make_unique<KPFSolution>(constructed.clone());

    float T = T_init;

    // Enquanto a temperatura for maior que a temperatura mínima 
    while (T > T_min) {
        int iterations_no_improvement = 0;
        for (int iter = 0; iter < ASmax; ++iter) {
            // Ao alcançar um número de iterações sem melhoria igual ao valor de relaxation
            // interrompemos a execução
            if (iterations_no_improvement >= relaxation) break;

            // Encontra um vizinho aleatório à solução atual
            KPFSolution neighbor = Get_Random_Neighbour(*s_current_ptr, gen);

            float obj_current = s_current_ptr->objectiveValue();
            float obj_neigh   = neighbor.objectiveValue();

            // Cálculo da diferença entre valores da função objetivo
            float delta = obj_current - obj_neigh;

            // Se a solução vizinho for melhor:
            if (delta <= 0.0f) {
                // Solução vizinha torna-se a atual
                s_current_ptr = make_unique<KPFSolution>(move(neighbor));
                // Se a solução atual for a melhor até o momento
                if (s_current_ptr->objectiveValue() > s_star_ptr->objectiveValue()) {
                    s_star_ptr = make_unique<KPFSolution>(*s_current_ptr);
                    iterations_no_improvement = 0;
                }
                // Caso contrário:
            } else {
                iterations_no_improvement += 1;
                float prob = exp(-delta / T);
                // Caso o valor aleatório for menor que essa probabilidade a solução vizinha torna-se a atual
                if (uni01(gen) < prob) {
                    s_current_ptr = make_unique<KPFSolution>(move(neighbor));
                }
            }
        }
        // Atualizamos a temperatura
        T *= alpha;
    }

    return *s_star_ptr;
}