#include "utils.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

                       
KPFSolution TabuSearch(const KPFSProblem& problem,int max_iter, int max_tabu_size, int relaxation) {
    
    // Geração de um número aleatório            
    random_device rd;
    mt19937 gen(rd());
    
    auto current_ptr = make_unique<KPFSolution>(ConstructiveAlgorithm(problem, 1, gen));
    auto best_ptr = make_unique<KPFSolution>(current_ptr->clone());

    int n = static_cast<int>(problem.items().size());
    
    // Criação de um vetor para identificar se um dado item é tabu
    vector<bool> is_tabu(n, false);
    vector<int> tabu_queue;
    tabu_queue.reserve(max_tabu_size);

    int no_impr_global = 0;
    for (int iter = 0; iter < max_iter; ++iter) {

        // Ao alcançar um número de iterações sem melhoria igual ao valor de relaxation
        // interrompemos a execução
        if (no_impr_global >= relaxation) break;

        unique_ptr<KPFSolution> best_cand_ptr;
        float best_cand_val = -numeric_limits<float>::infinity();
        int best_move = -1;

        for (int i = 0; i < n; ++i) {
            // Não realiza mudanças em itens tabu
            if (is_tabu[i]) continue;
            auto neigh_ptr = make_unique<KPFSolution>(current_ptr->clone());
            // Modifica o estado do item
            neigh_ptr->toggleItem(i);
            float v = neigh_ptr->objectiveValue();
            if (v > best_cand_val) {
                best_cand_val = v;
                // Guarda o índice do item modificado para incluí-lo na lista tabu
                best_move = i;
                best_cand_ptr = move(neigh_ptr);
            }
        }

        if (!best_cand_ptr) break;

        current_ptr = move(best_cand_ptr);
        // Se a solução atual for a melhor até o momento, atualize best_ptr 
        if (current_ptr->objectiveValue() > best_ptr->objectiveValue()) {
            best_ptr = make_unique<KPFSolution>(current_ptr->clone());
            no_impr_global = 0;
        } else {
            ++no_impr_global;
        }
        
        // Insere o item do melhor movimento na tabu_queue
        tabu_queue.push_back(best_move);
        is_tabu[best_move] = true;
        
        // Remove primeiro item da tabu_queue se o tamanho da fila superar o max_tabu_size
        if (static_cast<int>(tabu_queue.size()) > max_tabu_size) {
            int expired = tabu_queue.front();
            tabu_queue.erase(tabu_queue.begin());
            is_tabu[expired] = false;
        }
    }

    return *best_ptr;
}
