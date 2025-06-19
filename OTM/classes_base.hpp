#ifndef CLASSES_BASE_HPP
#define CLASSES_BASE_HPP

#include <vector>
#include <set>
#include <cstddef>

struct Item {
    std::size_t index;
    double profit;
    double weight;
    Item(std::size_t idx, double p, double w);
};

struct PenaltySet {
    std::set<std::size_t> indices;
    std::size_t allowance;
    double penalty;
    PenaltySet(const std::vector<std::size_t>& idxs,
               std::size_t allow,
               double pen);
};

class KPFSProblem {
public:
    KPFSProblem(const std::vector<Item>& items,
                const std::vector<PenaltySet>& penaltySets,
                double capacity,
                std::size_t maxViolations);

    const std::vector<Item>& items() const;
    const std::vector<PenaltySet>& penaltySets() const;
    double capacity() const;
    std::size_t maxViolations() const;
    const std::vector<std::vector<std::size_t>>& itemToSets() const;

private:
    std::vector<Item> items_;
    std::vector<PenaltySet> penalty_sets_;
    double capacity_;
    std::size_t max_violations_;
    std::vector<std::vector<std::size_t>> item_to_sets_;
    void buildItemIndex();
};

class KPFSolution {
public:
    explicit KPFSolution(const KPFSProblem& problem);

    // incremental updates (enforcing capacity & global k)
    bool addItem(std::size_t idx);
    bool removeItem(std::size_t idx);
    void toggleItem(std::size_t idx);

    // clone solution
    KPFSolution clone() const;

    // objective
    double objectiveValue() const;

    // getters
    const std::vector<int>& x() const;
    double totalProfit() const;
    double totalWeight() const;
    const std::vector<std::size_t>& penaltyViolations() const;
    double totalPenalty() const;
    std::size_t totalViolationCount() const;

private:
    const KPFSProblem& problem_;
    std::vector<int> x_;
    double total_profit_;
    double total_weight_;
    std::vector<std::size_t> penalty_violations_;
    double total_penalty_;
    std::size_t total_violation_count_;

    // helper: raw counts per penalty-set
    std::vector<std::size_t> penalty_counts_;

    // full recompute
    void recalculateAll();
};
#endif // CLASSES_BASE_HPP
