#include "classes_base.hpp"
#include <numeric>
#include <limits>

// Item & PenaltySet ctors
Item::Item(std::size_t idx, double p, double w)
    : index(idx), profit(p), weight(w) {}

PenaltySet::PenaltySet(const std::vector<std::size_t>& idxs,
                       std::size_t allow,
                       double pen)
    : mask(), allowance(allow), penalty(pen) { for (auto i : idxs) if (i < MAX_ITEMS) mask.set(i); }

// KPFSProblem ctor & build index
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

void KPFSProblem::buildItemIndex() {
    // one vector per item
    item_to_sets_.assign(items_.size(), {});
    for (std::size_t s = 0; s < penalty_sets_.size(); ++s) {
        for (std::size_t i = 0; i < items_.size(); ++i) {
            if (penalty_sets_[s].mask.test(i)) item_to_sets_[i].push_back(s);
        }
    }
}

// KPFSProblem getters

const std::vector<Item>& KPFSProblem::items() const           { return items_; }
const std::vector<PenaltySet>& KPFSProblem::penaltySets() const{ return penalty_sets_; }
double KPFSProblem::capacity() const                          { return capacity_; }
std::size_t KPFSProblem::maxViolations() const                { return max_violations_; }
const std::vector<std::vector<std::size_t>>& KPFSProblem::itemToSets() const {
    return item_to_sets_;
}

// KPFSolution
KPFSolution::KPFSolution(const KPFSProblem& problem)
  : problem_(problem), x_(), total_profit_(0), 
  total_weight_(0), penalty_violations_(problem.penaltySets().size(), 0), 
  total_penalty_(0), total_violation_count_(0), penalty_counts_(problem.penaltySets().size(), 0) 
  { recalculateAll(); }

void KPFSolution::recalculateAll() {
    total_profit_ = total_weight_ = 0;
    std::fill(penalty_counts_.begin(), penalty_counts_.end(), 0);
    std::fill(penalty_violations_.begin(), penalty_violations_.end(), 0);
    total_penalty_ = total_violation_count_ = 0;

    for (std::size_t i = x_. _Find_first(); i < MAX_ITEMS; i = x_. _Find_next(i)) {
        if (i >= problem_.items().size()) break;
        const auto& it = problem_.items()[i];
        total_profit_ += it.profit;
        total_weight_ += it.weight;
        for (auto s : problem_.itemToSets()[i])
            ++penalty_counts_[s];
    }
    // violations
    for (std::size_t s = 0; s < penalty_counts_.size(); ++s) {
        const auto& ps = problem_.penaltySets()[s];
        if (penalty_counts_[s] > ps.allowance) {
            auto excess = penalty_counts_[s] - ps.allowance;
            penalty_violations_[s] = excess;
            total_penalty_       += excess * ps.penalty;
            total_violation_count_ += excess;
        }
    }
}

bool KPFSolution::addItem(std::size_t idx) {
    if (idx >= MAX_ITEMS || x_.test(idx)) return false;
    const auto& it = problem_.items()[idx];    // enforce capacity
    if (total_weight_ + it.weight > problem_.capacity()) return false;

    // determine new violations
    std::size_t newViol = 0;
    for (auto s : problem_.itemToSets()[idx]) {
        if (penalty_counts_[s] + 1 > problem_.penaltySets()[s].allowance)
            newViol++;
    }
    if (total_violation_count_ + newViol > problem_.maxViolations())
        return false;

    x_.set(idx);
    total_profit_ += it.profit;
    total_weight_ += it.weight;
    for (auto s : problem_.itemToSets()[idx]) {
        auto prev = penalty_counts_[s]++;
        const auto& ps = problem_.penaltySets()[s];
        if (prev + 1 > ps.allowance) {
            ++penalty_violations_[s];
            total_penalty_       += ps.penalty;
            ++total_violation_count_;
        }
    }
    return true;
}

bool KPFSolution::removeItem(std::size_t idx) {
    if (idx >= MAX_ITEMS || !x_.test(idx)) return false;
    const auto& it = problem_.items()[idx];
     x_.reset(idx);
    total_profit_ -= it.profit;
    total_weight_ -= it.weight;
    for (auto s : problem_.itemToSets()[idx]) {
        auto prev = penalty_counts_[s]--;
        auto const& ps = problem_.penaltySets()[s];
        if (prev > ps.allowance) {
            penalty_violations_[s]--;
            total_penalty_         -= ps.penalty;
            total_violation_count_--;
        }
    }
    return true;
}

void KPFSolution::toggleItem(std::size_t idx) {
    if (x_.test(idx)) removeItem(idx);
    else           addItem(idx);
}

KPFSolution KPFSolution::clone() const {
    return *this;
}

// getters
const KPFSProblem& KPFSolution::problem() const { return problem_; }
const std::bitset<MAX_ITEMS>& KPFSolution::x() const { return x_; }
double KPFSolution::totalProfit() const                       { return total_profit_; }
double KPFSolution::totalWeight() const                       { return total_weight_; }
const std::vector<std::size_t>& KPFSolution::penaltyViolations() const { return penalty_violations_; }
double KPFSolution::totalPenalty() const                      { return total_penalty_; }
std::size_t KPFSolution::totalViolationCount() const           { return total_violation_count_; }

double KPFSolution::objectiveValue() const { return total_profit_ - total_penalty_; }

double KPFSolution::evaluateAddItem(std::size_t idx) const {
    if (idx >= problem_.items().size() || x_.test(idx)) return -std::numeric_limits<double>::infinity();

    const auto& it = problem_.items()[idx];
    if (total_weight_ + it.weight > problem_.capacity()) return -std::numeric_limits<double>::infinity();

    size_t newViol = 0;
    double temp_penalty = total_penalty_;
    for (auto s : problem_.itemToSets()[idx]) {
        size_t count = penalty_counts_[s];
        const auto& ps = problem_.penaltySets()[s];
        if (count >= ps.allowance) {
            newViol++;
            temp_penalty += ps.penalty;
        }
    }
    if (total_violation_count_ + newViol > problem_.maxViolations())
        return -std::numeric_limits<double>::infinity();

    return total_profit_ + it.profit - temp_penalty;
}

