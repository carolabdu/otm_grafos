#include "utils.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <memory>
#include <algorithm>

using namespace std;

KPFSolution ConstructiveAlgorithm(const KPFSProblem& problem, float alpha) {
    KPFSolution solution(problem);  // Inicio com solução vazia
    std::vector<int> remaining_items(problem.items().size());
    std::iota(remaining_items.begin(), remaining_items.end(), 0);

    std::mt19937 rng(std::random_device{}());

    while (!remaining_items.empty()) {
        // 1. Calcula a heurística h(i) = lucro/peso para todos os itens restantes
        std::vector<std::pair<int, float>> heuristics;
        for (int idx : remaining_items) {
            const Item& item = problem.items()[idx];
            if (item.weight == 0) continue; 
            float heuristic = static_cast<float>(item.profit) / item.weight;
            heuristics.emplace_back(idx, heuristic);
        }

        if (heuristics.empty()) break;

        // 2. Cálculo de h_min and h_max
        float h_min = std::numeric_limits<float>::infinity();
        float h_max = -std::numeric_limits<float>::infinity();
        for (const auto& [_, h] : heuristics) {
            if (h < h_min) h_min = h;
            if (h > h_max) h_max = h;
        }

        // 3. Construção da RCL: itens com h(i) >= h_min + α*(h_max - h_min)
        float threshold = h_min + alpha * (h_max - h_min);
        std::vector<int> rcl;
        for (const auto& [idx, h] : heuristics) {
            if (h >= threshold) {
                rcl.push_back(idx);
            }
        }

        if (rcl.empty()) break;

        // 4. Seleção aleatória de um item da RCL
        std::uniform_int_distribution<int> dist(0, rcl.size() - 1);
        int selected = rcl[dist(rng)];

        // 5. Tentativa de adicionar o item (toggle) and checagem da viabilidade
        solution.toggleItem(selected);
        if (solution.totalWeight() > problem.capacity() ||
            solution.totalViolationCount() > problem.maxViolations()) {
            // Revert toggle if infeasible
            solution.toggleItem(selected);
        }

        // 6. Remove o item selecionado do conjunto de candidatos
        remaining_items.erase(std::remove(remaining_items.begin(), remaining_items.end(), selected), remaining_items.end());
    }

    return solution;
}


KPFSolution BestImprovement(const KPFSolution& original_solution) {
    auto best_ptr = make_unique<KPFSolution>(original_solution.clone());
    float best_obj_value = best_ptr->objectiveValue();

    const int n = static_cast<int>(original_solution.x().size());
    bool improved = true;

    while (improved) {
        improved = false;

        vector<unique_ptr<KPFSolution>> neighborhood;
        neighborhood.reserve(n);
        for (int i = 0; i < n; ++i) {
            auto neighbour = make_unique<KPFSolution>(best_ptr->clone());
            neighbour->toggleItem(i);
            neighborhood.push_back(move(neighbour));
        }

        for (auto& nb_ptr : neighborhood) {
            float val = nb_ptr->objectiveValue();
            if (val > best_obj_value) {
                best_obj_value = val;
                best_ptr = std::move(nb_ptr);
                improved = true;
            }
        }
    }

    return *best_ptr;
}