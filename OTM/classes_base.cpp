#include "classes_base.hpp"
#include <vector>
#include <set>
#include <cstddef>
#include <limits>

Item::Item(std::size_t idx, double p, double w)
    : index(idx), profit(p), weight(w) {}


PenaltySet::PenaltySet(const std::vector<std::size_t>& idxs,
                       std::size_t allow,
                       double pen)
    : indices(idxs.begin(), idxs.end()), allowance(allow), penalty(pen) {}


KPFSProblem::KPFSProblem(const std::vector<Item>& items,
                         const std::vector<PenaltySet>& penaltySets,
                         double capacity,
                         std::size_t maxViolations)
    : items_(items),
      penalty_sets_(penaltySets),
      capacity_(capacity),
      max_violations_(maxViolations)
{
    buildItemIndex();
}

const std::vector<Item>& KPFSProblem::items() const { return items_; }
const std::vector<PenaltySet>& KPFSProblem::penaltySets() const { return penalty_sets_; }
double KPFSProblem::capacity() const { return capacity_; }
std::size_t KPFSProblem::maxViolations() const { return max_violations_; }
const std::vector<std::vector<std::size_t>>& KPFSProblem::itemToSets() const { return item_to_sets_; }

void KPFSProblem::buildItemIndex() {
    item_to_sets_.assign(items_.size(), {});
    for (std::size_t setIdx = 0; setIdx < penalty_sets_.size(); ++setIdx) {
        for (auto itemIdx : penalty_sets_[setIdx].indices) {
            if (itemIdx < item_to_sets_.size()) {
                item_to_sets_[itemIdx].push_back(setIdx);
            }
        }
    }
}

KPFSolution::KPFSolution(const KPFSProblem& problem)
    : problem_(problem),
      x_(problem.items().size(), 0),
      total_profit_(0),
      total_weight_(0),
      penalty_violations_(problem.penaltySets().size(), 0),
      total_penalty_(0),
      total_violation_count_(0)
{
    recalculateAll();
}

void KPFSolution::toggleItem(std::size_t index) {
    if (index < x_.size()) {
        x_[index] = 1 - x_[index];
        recalculateAll();
    }
}

KPFSolution KPFSolution::clone() const {
    KPFSolution copy(problem_);
    copy.x_ = x_;
    copy.recalculateAll();
    return copy;
}

double KPFSolution::objectiveValue() const {
    if (total_weight_ > problem_.capacity() ||
        total_violation_count_ > problem_.maxViolations()) {
        return -std::numeric_limits<double>::infinity();
    }
    return total_profit_ - total_penalty_;
}

const std::vector<int>& KPFSolution::x() const { return x_; }
double KPFSolution::totalProfit() const { return total_profit_; }
double KPFSolution::totalWeight() const { return total_weight_; }
const std::vector<std::size_t>& KPFSolution::penaltyViolations() const { return penalty_violations_; }
double KPFSolution::totalPenalty() const { return total_penalty_; }
std::size_t KPFSolution::totalViolationCount() const { return total_violation_count_; }

void KPFSolution::recalculateAll() {
    total_profit_ = 0;
    total_weight_ = 0;
    std::vector<std::size_t> counts(problem_.penaltySets().size(), 0);

    for (std::size_t idx = 0; idx < x_.size(); ++idx) {
        if (x_[idx]) {
            const Item& item = problem_.items()[idx];
            total_profit_ += item.profit;
            total_weight_ += item.weight;
            for (auto setIdx : problem_.itemToSets()[idx]) {
                counts[setIdx]++;
            }
        }
    }

    total_penalty_ = 0;
    total_violation_count_ = 0;
    for (std::size_t i = 0; i < counts.size(); ++i) {
        std::size_t excess = 0;
        if (counts[i] > problem_.penaltySets()[i].allowance) {
            excess = counts[i] - problem_.penaltySets()[i].allowance;
        }
        penalty_violations_[i] = excess;
        total_penalty_ += excess * problem_.penaltySets()[i].penalty;
        total_violation_count_ += excess;
    }
}
