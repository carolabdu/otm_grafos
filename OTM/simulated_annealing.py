import math
import random
from utils import ConstructiveAlgorithm, RandomSolutions
from classes_solucao import KPFSolution

def get_random_neighbor(solution):
    """Gera um vizinho aleatório por 1-flip (inversão de um item)."""
    neighbor = solution.clone()
    index = random.randint(0, len(solution.x) - 1)
    neighbor.toggle_item(index)
    return neighbor

def simulated_annealing(problem, ASmax=100, T_min=1e-3, T_init=100.0, alpha=0.95):
    """
    Simulated Annealing para o problema KPFS.
    
    Args:
        problem: instância de KPFSProblem.
        ASmax: número máximo de iterações por temperatura.
        T_min: temperatura mínima (critério de parada).
        T_init: temperatura inicial.
        alpha: fator de resfriamento (ex: 0.95).
    
    Returns:
        Melhor solução encontrada (KPFSolution).
    """
    # Geração inicial com RCL
    initial_solutions = RandomSolutions(problem, size=10, randomness=0.5)
    s_star = ConstructiveAlgorithm(initial_solutions)
    s_current = s_star.clone()
    T = T_init

    while T > T_min:
        for _ in range(ASmax):
            s_neighbor = get_random_neighbor(s_current)
            delta = s_current.objective_value() - s_neighbor.objective_value()

            if delta <= 0:  # s_neighbor é melhor ou igual
                s_current = s_neighbor
                if s_current.objective_value() > s_star.objective_value():
                    s_star = s_current.clone()
            else:
                prob = math.exp(-delta / T)
                if random.random() < prob:
                    s_current = s_neighbor  # aceita pior com certa probabilidade

        T *= alpha  # resfriamento

    return s_star
