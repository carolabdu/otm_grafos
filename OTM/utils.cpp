#include "utils.hpp"
#include "classes_base.hpp"
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>
#include <memory>

using namespace std;

KPFSolution ConstructiveAlgorithm(const KPFSProblem& problem, float alpha) {
    KPFSolution solution(problem);
    vector<int> remaining(problem.items().size());
    iota(remaining.begin(), remaining.end(), 0);
    mt19937 rng(random_device{}());

    while (!remaining.empty()) {
        vector<pair<int,double>> cand;
        double best = -numeric_limits<double>::infinity();
        double worst =  numeric_limits<double>::infinity();
        for (int idx : remaining) {
            auto temp = solution.clone();
            temp.addItem(idx);
            double val = temp.objectiveValue();
            best  = max(best, val);
            worst = min(worst, val);
            cand.emplace_back(idx,val);
        }
        double threshold = best - alpha*(best-worst);
        vector<int> rcl;
        for (auto &p : cand) if (p.second >= threshold) rcl.push_back(p.first);
        uniform_int_distribution<size_t> dist(0,rcl.size()-1);
        int chosen = rcl[dist(rng)];
        solution.addItem(chosen);
        remaining.erase(remove(remaining.begin(), remaining.end(), chosen), remaining.end());
    }
    return solution;
}

KPFSolution BestImprovement(const KPFSolution& initSol) {
    // use pointer to avoid assignment operator
    auto bestPtr = make_unique<KPFSolution>(initSol);
    bool improved = true;
    while (improved) {
        improved = false;
        double currVal = bestPtr->objectiveValue();
        unique_ptr<KPFSolution> localPtr = make_unique<KPFSolution>(*bestPtr);
        // additions
        for (size_t i=0;i<localPtr->x().size();++i) if (!localPtr->x()[i]) {
            auto nb = make_unique<KPFSolution>(*localPtr);
            nb->addItem(i);
            double v = nb->objectiveValue();
            if (v > currVal) {
                bestPtr = move(nb);
                currVal = v;
                improved = true;
            }
        }
        // removals
        for (size_t i=0;i<localPtr->x().size();++i) if (localPtr->x()[i]) {
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
    return *bestPtr;
}

