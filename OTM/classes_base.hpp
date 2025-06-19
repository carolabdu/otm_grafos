#ifndef CLASSES_BASE_HPP
#define CLASSES_BASE_HPP

#include <vector>
#include <set>
#include <cstddef>
#include <bitset>
static constexpr std::size_t MAX_ITEMS = 1000;

struct Item {
    std::size_t index;
    double profit;
    double weight;
    Item(std::size_t idx, double p, double w);
};

struct PenaltySet {
    std::bitset<MAX_ITEMS> mask;
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
    void buildPenaltyMasks();
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
    double evaluateAddItem(std::size_t idx) const;


    // getters
    const KPFSProblem& problem() const;
    const std::bitset<MAX_ITEMS>& x() const;
    double totalProfit() const;
    double totalWeight() const;
    const std::vector<std::size_t>& penaltyViolations() const;
    double totalPenalty() const;
    std::size_t totalViolationCount() const;

private:
    const KPFSProblem& problem_;
    std::bitset<MAX_ITEMS> x_; 
    double total_profit_;
    double total_weight_;
    std::vector<std::size_t> penalty_violations_;
    double total_penalty_;
    std::size_t total_violation_count_;

    std::vector<std::size_t> penalty_counts_;

    void recalculateAll();
};
#endif