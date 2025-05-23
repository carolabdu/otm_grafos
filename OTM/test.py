import random
import time
from read_txt import load_kpfs_instance
from grasp import grasp
from classes_solucao import KPFSolution
from utils import ConstructiveAlgorithm, BestImprovement, RandomSolutions
from classes_base import KPFSProblem, Item, PenaltySet  # Importando as classes corretamente
from ILS import ils
from simulated_annealing import simulated_annealing
from tabu import tabu_search

# Criando uma instância do problema
items = [
    Item(0, profit=10, weight=5),
    Item(1, profit=20, weight=10),
    Item(2, profit=15, weight=7)
]

penalty_sets = [
    PenaltySet(indices=[0, 1], allowance=1, penalty=5),
    PenaltySet(indices=[1, 2], allowance=2, penalty=3)
]

problem = load_kpfs_instance('instances/scenario1/correlated_sc1/300/kpfs_1.txt')

solutions = RandomSolutions(problem, size=5)

# Testando critério de parada: máximo de iterações
print("\nTestando critério de parada: máximo de iterações")
best_solution_iterations = grasp("max_iterations", 10, solutions, alpha=0.2)
print(f"Melhor solução encontrada: {best_solution_iterations.objective_value()}")

# Testando critério de parada: tempo máximo
print("\nTestando critério de parada: tempo máximo")
start_time = time.time()
best_grasp = grasp("max_time", 2, solutions, alpha=0.2)  # Tempo máximo de 2 segundos
print(f"Melhor solução pelo GRASP: {best_grasp.objective_value()}")
best_ils = ils(problem, max_iter=50, perturbation_strength=2, alpha=0.2)
print(f"Melhor solução pelo ILS: {best_ils.objective_value()}")
best_sa = simulated_annealing(problem, ASmax=100, T_min=1e-3, T_init=100, alpha=0.95)
print(f"Melhor solução pelo SA: {best_sa.objective_value()}")
best_tabu = tabu_search(problem, max_iter=100, max_tabu_size=10)
print(f"Melhor solução pelo Tabu Search: {best_tabu.objective_value()}")
elapsed_time = time.time() - start_time
best_solution_time = min(best_grasp.objective_value(), best_ils.objective_value(), best_sa.objective_value(), best_tabu.objective_value())
print(f"Tempo gasto: {elapsed_time:.2f}s, Melhor solução encontrada: {best_solution_time}")

# Testando critério de parada: número máximo de iterações sem melhoria
print("\nTestando critério de parada: número máximo de iterações sem melhoria")
best_solution_no_improvement = grasp("no_improvement", 5, solutions, alpha=0.2)
print(f"Melhor solução encontrada: {best_solution_no_improvement.objective_value()}")
