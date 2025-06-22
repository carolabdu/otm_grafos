#include "utils.hpp"
#include "classes_base.hpp"
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <memory>
#include <iostream>

using namespace std;

KPFSolution ConstructiveAlgorithm(const KPFSProblem& problem, float alpha, mt19937& rng) {
    KPFSolution solution(problem);
    vector<int> remaining(problem.items().size());
    // Criação de um vetor para representar os índices dos itens na mochila [0, 1, i, ..., nI-1]
    iota(remaining.begin(), remaining.end(), 0);

    while (!remaining.empty()) {
    // Calcula a heurística h(i) = lucro/peso para todos os itens restantes
        vector<pair<int, float>> heuristics;
        for (int idx : remaining) {
            const Item& item = problem.items()[idx];
            if (item.weight == 0) continue; 
            float heuristic = static_cast<float>(item.profit) / item.weight;
            heuristics.emplace_back(idx, heuristic);
        }
        if (heuristics.empty()) break;
        
        // Cálculo de h_min and h_max
        float h_min = numeric_limits<float>::infinity();
        float h_max = -numeric_limits<float>::infinity();
        for (const auto& [_, h] : heuristics) {
            h_min = min(h_min, h);
            h_max = max(h_max, h);
        }

        // Construção da RCL: itens com h(i) >= h_min + α*(h_max - h_min)
        double threshold = h_min + alpha*(h_max-h_min);
        vector<int> rcl;
        for (const auto& [idx, h] : heuristics) {
            if (h >= threshold) {
                rcl.push_back(idx);
            }
        }

        if (rcl.empty()) break;

        // Seleção aleatória de um item da RCL
        uniform_int_distribution<size_t> dist(0, rcl.size()-1);
        int chosen = rcl[dist(rng)];

        // Tentativa de adicionar o item e checagem da viabilidade
        solution.addItem(chosen);

        // Remove o item selecionado do conjunto de candidatos
        remaining.erase(remove(remaining.begin(), remaining.end(), chosen), remaining.end());
    }
    return solution;
}



KPFSolution BestImprovement(const KPFSolution& initSol) {
    auto bestPtr = make_unique<KPFSolution>(initSol);
    bool improved = true;
    while (improved) {
        improved = false;
        double currVal = bestPtr->objectiveValue();
        unique_ptr<KPFSolution> localPtr = make_unique<KPFSolution>(*bestPtr);
        
        for (size_t i = 0; i < localPtr->x().size(); ++i) {
            // Adiciona na mochila um item que melhora o valor da função objetivo
            if (!localPtr->x()[i]) {
                double v = localPtr->evaluateAddItem(i);
                if (v > currVal) {
                    auto nb = make_unique<KPFSolution>(*localPtr);
                    nb->addItem(i);
                    bestPtr = move(nb);
                    currVal = v;
                    improved = true;
                }
            }
            // Remove da mochila um item caso sua remoção melhore o valor da função objetivo
            else{
                auto nb = make_unique<KPFSolution>(*localPtr);
                nb->removeItem(i);
                double v = nb->objectiveValue();
                if (v > currVal) {
                    bestPtr = move(nb);
                    currVal = v;
                    improved = true;
                }
            }
        }

    }
    return *bestPtr;
}
