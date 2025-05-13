import random
import copy

# Representa um item com lucro e peso
class Item:
    def __init__(self, index, profit, weight):
        self.index = index
        self.profit = profit
        self.weight = weight


# Representa um conjunto de penalidade (subconjunto de itens)
class PenaltySet:
    def __init__(self, indices, allowance, penalty):
        self.indices = set(indices)        # índice dos itens incluídos nesse conjunto
        self.allowance = allowance         # número de itens permitidos sem penalidade
        self.penalty = penalty             # penalidade unitaria por excesso


# Representa a instância do problema KPFS
class KPFSProblem:
    def __init__(self, items, penalty_sets, capacity, max_violations):
        self.items = items                            # lista de objetos Item
        self.penalty_sets = penalty_sets              # lista de objetos PenaltySet
        self.capacity = capacity                      # capacidade da mochila
        self.max_violations = max_violations          # número total permitido de violações
        self.item_to_sets = self._build_item_index()  # mapeia cada item aos conjuntos que ele pertence

    def _build_item_index(self):
        """Constrói um índice reverso: mapeia cada item aos conjuntos de penalidade que o contêm."""
        mapping = {i: [] for i in range(len(self.items))}
        for idx, pset in enumerate(self.penalty_sets):
            for i in pset.indices:
                mapping[i].append(idx)
        return mapping
