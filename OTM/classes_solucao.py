# Representa uma solução candidata do problema
class KPFSolution:
    def __init__(self, problem):
        self.problem = problem
        self.x = [0] * len(problem.items)             # vetor de decisão (0/1 para cada item)
        self.total_profit = 0
        self.total_weight = 0
        self.penalty_violations = [0] * len(problem.penalty_sets)  # violações por conjunto
        self.total_penalty = 0
        self.total_violation_count = 0
        self._recalculate_all()                       # inicializa os valores

    def _recalculate_all(self):
        """Recalcula lucro, peso e penalidades da solução do zero."""
        self.total_profit = 0
        self.total_weight = 0
        counts = [0] * len(self.problem.penalty_sets)

        for idx, selected in enumerate(self.x):
            if selected:
                item = self.problem.items[idx]
                self.total_profit += item.profit
                self.total_weight += item.weight
                for set_idx in self.problem.item_to_sets[idx]:
                    counts[set_idx] += 1

        self.penalty_violations = []
        self.total_penalty = 0
        self.total_violation_count = 0

        # Calcula as violações e penalidades
        for i, count in enumerate(counts):
            excess = max(0, count - self.problem.penalty_sets[i].allowance)
            self.penalty_violations.append(excess)
            self.total_penalty += excess * self.problem.penalty_sets[i].penalty
            self.total_violation_count += excess

    def toggle_item(self, index):
        """Ativa ou desativa o item no vetor x e atualiza os valores."""
        self.x[index] ^= 1  # inverte 0↔1
        self._recalculate_all()

    def clone(self):
        """Retorna uma cópia independente da solução atual."""
        copy_sol = KPFSolution(self.problem)
        copy_sol.x = self.x[:]
        copy_sol._recalculate_all()
        return copy_sol

    def objective_value(self):
        """Retorna o valor da função objetivo se a solução for válida, ou -∞ se inválida."""
        if self.total_weight > self.problem.capacity or self.total_violation_count > self.problem.max_violations:
            return -float('inf')
        return self.total_profit - self.total_penalty
