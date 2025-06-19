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

    // Pick a random valid index
    std::uniform_int_distribution<> distrib(0, num_items - 1);
    int index = distrib(gen);

    neighbour.toggleItem(index);
    return neighbour;
}


KPFSolution Simulated_Annealing(KPFSProblem problem, int ASmax, float T_min, float T_init, float alpha, int relaxation){
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> uni01(0.0f, 1.0f);
    KPFSolution constructed = ConstructiveAlgorithm(problem, 1, gen);

    auto s_star_ptr    = make_unique<KPFSolution>(constructed.clone());
    auto s_current_ptr = make_unique<KPFSolution>(constructed.clone());

    float T = T_init;

    while (T > T_min) {
        int iterations_no_improvement = 0;
        for (int iter = 0; iter < ASmax; ++iter) {

            if (iterations_no_improvement >= relaxation) break;
            KPFSolution neighbor = Get_Random_Neighbour(*s_current_ptr, gen);

            float obj_current = s_current_ptr->objectiveValue();
            float obj_neigh   = neighbor.objectiveValue();
            float delta       = obj_current - obj_neigh;

            if (delta <= 0.0f) {
                s_current_ptr = make_unique<KPFSolution>(move(neighbor));

                if (s_current_ptr->objectiveValue() > s_star_ptr->objectiveValue()) {
                    s_star_ptr = make_unique<KPFSolution>(*s_current_ptr);
                    iterations_no_improvement = 0;
                }
            } else {
                iterations_no_improvement += 1;
                float prob = exp(-delta / T);
                if (uni01(gen) < prob) {
                    s_current_ptr = make_unique<KPFSolution>(move(neighbor));
                }
            }
        }

        T *= alpha;
    }

    return *s_star_ptr;
}