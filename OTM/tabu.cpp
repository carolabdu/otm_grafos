#include "utils.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

                       
KPFSolution TabuSearch(const KPFSProblem& problem,
                       int max_iter,
                       int max_tabu_size,
                       int relaxation) {
    random_device rd;
    mt19937 gen(rd());
    auto current_ptr = make_unique<KPFSolution>(ConstructiveAlgorithm(problem, 1, gen));
    auto best_ptr    = make_unique<KPFSolution>(current_ptr->clone());

    int n = static_cast<int>(problem.items().size()); // use actual number of items
    vector<bool> is_tabu(n, false);
    vector<int> tabu_queue;
    tabu_queue.reserve(max_tabu_size);

    int no_impr_global = 0;
    for (int iter = 0; iter < max_iter; ++iter) {
        if (no_impr_global >= relaxation) break;

        unique_ptr<KPFSolution> best_cand_ptr;
        float best_cand_val = -numeric_limits<float>::infinity();
        int best_move = -1;

        for (int i = 0; i < n; ++i) {
            if (is_tabu[i]) continue;
            auto neigh_ptr = make_unique<KPFSolution>(current_ptr->clone());
            neigh_ptr->toggleItem(i);
            float v = neigh_ptr->objectiveValue();
            if (v > best_cand_val) {
                best_cand_val = v;
                best_move = i;
                best_cand_ptr = move(neigh_ptr);
            }
        }

        if (!best_cand_ptr) break;

        current_ptr = move(best_cand_ptr);

        if (current_ptr->objectiveValue() > best_ptr->objectiveValue()) {
            best_ptr = make_unique<KPFSolution>(current_ptr->clone());
            no_impr_global = 0;
        } else {
            ++no_impr_global;
        }

        tabu_queue.push_back(best_move);
        is_tabu[best_move] = true;
        if (static_cast<int>(tabu_queue.size()) > max_tabu_size) {
            int expired = tabu_queue.front();
            tabu_queue.erase(tabu_queue.begin());
            is_tabu[expired] = false;
        }
    }

    return *best_ptr;
}
