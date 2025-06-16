#include "utils.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <memory>
#include <algorithm>

using namespace std;

KPFSolution ConstructiveAlgorithm(vector<KPFSolution> solutions, float alpha){

    float obj_value;
    float h_min = 1e20;
    float h_max = -1e20;
    for (KPFSolution& solution : solutions){
        obj_value = solution.objectiveValue();
        if (obj_value < h_min) h_min = obj_value;
        else{
            if (obj_value > h_max) h_max = obj_value;
        }
    }
    float lower_range = h_min;
    float upper_range = h_max + alpha * (h_min - h_max);
    vector<KPFSolution> candidates_list;
    for (KPFSolution& solution : solutions){
        obj_value = solution.objectiveValue();
        if (obj_value <= upper_range && obj_value >= lower_range){
            candidates_list.push_back(solution);
        }
    }
    srand(time(0));
    int randomIndex = std::rand() % candidates_list.size();

    auto it = candidates_list.begin();
    advance(it, randomIndex);
    return  *it;

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

vector<KPFSolution> RandomSolutions(const KPFSProblem& problem, int size) {
    vector<KPFSolution> solutions;
    solutions.reserve(size);

    random_device rd;
    mt19937 gen(rd());

    for (int s = 0; s < size; ++s) {
        KPFSolution sol(problem); 
        vector<int> available_item_indices(problem.items().size());
        for (int i = 0; i < static_cast<int>(available_item_indices.size()); ++i) {
            available_item_indices[i] = i;
        }
        shuffle(available_item_indices.begin(), available_item_indices.end(), gen);
        for (int item_idx : available_item_indices) {
            sol.toggleItem(item_idx);
            if (sol.totalWeight() > problem.capacity() || sol.totalViolationCount() > problem.maxViolations()) {
                sol.toggleItem(item_idx);
            }
        }
        
        solutions.push_back(sol);
    }
    return solutions;
}