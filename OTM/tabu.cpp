#include "utils.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

                       
KPFSolution TabuSearch(const KPFSProblem& problem,
                       int max_iter,
                       int max_tabu_size)
{
    KPFSolution initial = ConstructiveAlgorithm(problem, 0); 

    auto s_prime_ptr = std::make_unique<KPFSolution>(initial.clone());
    auto best_ptr    = std::make_unique<KPFSolution>(*s_prime_ptr);

    std::vector<int> tabu_list;
    tabu_list.reserve(max_tabu_size);

    for (int iter = 0; iter < max_iter; ++iter) {
        std::unique_ptr<KPFSolution> best_cand_ptr = nullptr;
        float best_cand_value = -std::numeric_limits<float>::infinity();
        int best_move = -1;

        int n = static_cast<int>(s_prime_ptr->x().size());
        for (int i = 0; i < n; ++i) {
            if (std::find(tabu_list.begin(), tabu_list.end(), i) != tabu_list.end())
                continue;

            auto neighbor_ptr = std::make_unique<KPFSolution>(s_prime_ptr->clone());
            neighbor_ptr->toggleItem(i);
            float val = neighbor_ptr->objectiveValue();

            if (val > best_cand_value) {
                best_cand_value = val;
                best_cand_ptr   = std::move(neighbor_ptr);
                best_move       = i;
            }
        }

        if (best_cand_ptr) {
            s_prime_ptr = std::move(best_cand_ptr);

            if (s_prime_ptr->objectiveValue() > best_ptr->objectiveValue()) {
                best_ptr = std::make_unique<KPFSolution>(*s_prime_ptr);
            }
            tabu_list.push_back(best_move);
            if (static_cast<int>(tabu_list.size()) > max_tabu_size) {
                tabu_list.erase(tabu_list.begin());
            }
        }
    }
    return *best_ptr;
}
