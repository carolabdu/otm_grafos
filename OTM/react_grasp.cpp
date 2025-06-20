#include <vector>
#include "react_grasp.hpp"
#include "utils.hpp"
#include <memory>
#include <random>
#include <iostream>

using namespace std;


KPFSolution React_GRASP(const KPFSProblem& problem, vector<float> alpha_list, int blockIter,int maxIterations, int relaxation) {

    int m = alpha_list.size();
    vector<float> probs(m, 1/m);
    vector<float> s(m, 0);
    vector<int> na(m,0);
    auto best_ptr = make_unique<KPFSolution>(problem);
    float best_value = best_ptr->objectiveValue();
    int iterations_no_improvement = 0;

    random_device rd;
    mt19937 gen(rd());
    int iter;
    for (iter = 0; iter < maxIterations; ++iter) {

        discrete_distribution<size_t> dist(probs.begin(), probs.end());
        int i = dist(gen);
        float alpha = alpha_list[i];
        na[i] +=1;
        KPFSolution current_solution = ConstructiveAlgorithm(problem, alpha, gen);
        // Busca Local
        auto improved_ptr = make_unique<KPFSolution>(BestImprovement(current_solution));
        float val = improved_ptr->objectiveValue();
        s[i] += val;
        if (val > best_value) {
                best_value = val;
                best_ptr = move(improved_ptr);
                iterations_no_improvement = 0;
            }
        else{
            iterations_no_improvement +=1;
        }
        if (iterations_no_improvement >= relaxation) break;
        }

        if (iter % blockIter == 0) {
            vector<float> A(m);
            for (int j = 0; j < m; ++j) {
                if (na[j] > 0)
                    A[j] = s[j] / float(na[j]);
                else
                    A[j] = numeric_limits<float>::infinity();  
            }
            vector<float> q(m);
            for (int j = 0; j < m; ++j) {
                q[j] = (A[j] > 0 && A[j] < numeric_limits<float>::infinity()) ? (best_value / A[j]) : 0.0f;
            }
            float sumQ = accumulate(q.begin(), q.end(), 0.0f);
            if (sumQ > 0) {
                for (int j = 0; j < m; ++j)
                    probs[j] = q[j] / sumQ;
            }
            // else leave p unchanged
        }
    
    // Return the best solution (by copy constructing)
    return *best_ptr;
}
