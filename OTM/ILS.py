from utils import ConstructiveAlgorithm, BestImprovement, RandomSolutions
from classes_solucao import KPFSolution
import random
import time

def get_neighborhood(solution):
    neighbors = []
    for i in range(len(solution.x)):
        neighbor = solution.clone()
        neighbor.toggle_item(i)
        neighbors.append(neighbor)
    return neighbors


def perturb_solution(solution, strength=1):
    """
    Aplica perturbação simples: inverte aleatoriamente 'strength' bits.
    """
    new_solution = solution.clone()
    n = len(solution.x)
    indices = random.sample(range(n), strength)
    for idx in indices:
        new_solution.toggle_item(idx)
    return new_solution


def ils(problem, max_iter=100, perturbation_strength=2, alpha=0.2):
    """
    ILS para o problema KPFS usando as funções já existentes.
    """
    # 1. Gera conjunto de soluções iniciais e seleciona uma com alpha (GRASP-style)
    initial_solutions = RandomSolutions(problem, size=10, randomness=0.5)
    current_solution = ConstructiveAlgorithm(initial_solutions, alpha=alpha)
    current_solution = BestImprovement(current_solution)

    best_solution = current_solution.clone()
    best_value = best_solution.objective_value()

    for _ in range(max_iter):
        # 2. Perturba a solução atual
        perturbed = perturb_solution(current_solution, strength=perturbation_strength)

        # 3. Busca local na solução perturbada
        improved = BestImprovement(perturbed)

        # 4. Aceita se melhorar
        if improved.objective_value() > best_value:
            best_solution = improved.clone()
            best_value = best_solution.objective_value()

        # Atualiza ponto de partida para próxima iteração (exploração contínua)
        current_solution = improved

    return best_solution
