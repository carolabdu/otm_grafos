import time
from grasp import grasp
from classes_solucao import KPFSolution
from utils import ConstructiveAlgorithm, BestImprovement
from classes_base import KPFSProblem, Item, PenaltySet  # Importando as classes corretamente

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

problem = KPFSProblem(items, penalty_sets, capacity=20, max_violations=1)

solutions = [KPFSolution(problem) for _ in range(5)]
neighbourhood = [sol.clone() for sol in solutions]

# Testando critério de parada: máximo de iterações
print("\nTestando critério de parada: máximo de iterações")
best_solution_iterations = grasp("max_iterations", 10, solutions, neighbourhood, alpha=0.2)
print(f"Melhor solução encontrada: {best_solution_iterations.objective_value()}")

# Testando critério de parada: tempo máximo
print("\nTestando critério de parada: tempo máximo")
start_time = time.time()
best_solution_time = grasp("max_time", 2, solutions, neighbourhood, alpha=0.2)  # Tempo máximo de 2 segundos
elapsed_time = time.time() - start_time
print(f"Tempo gasto: {elapsed_time:.2f}s, Melhor solução encontrada: {best_solution_time.objective_value()}")

# Testando critério de parada: número máximo de iterações sem melhoria
print("\nTestando critério de parada: número máximo de iterações sem melhoria")
best_solution_no_improvement = grasp("no_improvement", 5, solutions, neighbourhood, alpha=0.2)
print(f"Melhor solução encontrada: {best_solution_no_improvement.objective_value()}")