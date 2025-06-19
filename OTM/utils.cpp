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
    iota(remaining.begin(), remaining.end(), 0);

    while (!remaining.empty()) {
        vector<pair<int,double>> cand;
        double h_max = -numeric_limits<double>::infinity();
        double h_min =  numeric_limits<double>::infinity();
        for (int idx : remaining) {
            double val = solution.evaluateAddItem(idx);
            h_max = max(h_max, val);
            h_min = min(h_min, val);
            cand.emplace_back(idx, val);
        }
        double threshold = h_min + alpha*(h_max-h_min);
        vector<int> rcl;
        for (auto &p : cand) if (p.second >= threshold) rcl.push_back(p.first);

        if (rcl.empty()) break;

        uniform_int_distribution<size_t> dist(0, rcl.size()-1);
        int chosen = rcl[dist(rng)];
        solution.addItem(chosen);
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

        // additions
        for (size_t i = 0; i < localPtr->x().size(); ++i) {
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
        }
        for (size_t i = 0; i < localPtr->x().size(); ++i) {
            if (localPtr->x()[i]) {
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
