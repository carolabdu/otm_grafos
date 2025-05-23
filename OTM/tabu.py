from classes_solucao import KPFSolution
from utils import ConstructiveAlgorithm, RandomSolutions
import random

def tabu_search(problem, max_iter=100, max_tabu_size=10):
    """
    Tabu Search para o problema KPFS.
    
    Args:
        problem: instância KPFSProblem.
        max_iter: critério de parada (nº máximo de iterações).
        max_tabu_size: tamanho máximo da lista tabu.
    
    Returns:
        Melhor solução encontrada (KPFSolution).
    """

    # === 1. Solução inicial ===
    solutions = RandomSolutions(problem, size=10)
    s_prime = ConstructiveAlgorithm(solutions)
    best = s_prime.clone()

    # === 2. Lista tabu com índices de item proibidos de flipar ===
    tabu_list = []
    
    for _ in range(max_iter):
        best_candidate = None
        best_candidate_value = -float('inf')

        # === 3. Geração da vizinhança por 1-flip ===
        for i in range(len(s_prime.x)):
            if i in tabu_list:
                continue  # movimento tabu

            neighbor = s_prime.clone()
            neighbor.toggle_item(i)
            value = neighbor.objective_value()

            if value > best_candidate_value:
                best_candidate = neighbor
                best_candidate_value = value
                best_move = i

        # === 4. Atualiza solução corrente e melhor global ===
        if best_candidate:
            s_prime = best_candidate
            if s_prime.objective_value() > best.objective_value():
                best = s_prime.clone()

            # === 5. Atualiza lista tabu ===
            tabu_list.append(best_move)
            if len(tabu_list) > max_tabu_size:
                tabu_list.pop(0)

    return best
