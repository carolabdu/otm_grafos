import random
import time
from utils import ConstructiveAlgorithm, BestImprovement
from classes_solucao import KPFSolution

def grasp(
        stopping_criterion,
        criterion_value,
        solutions,
        neighbourhood,
        alpha=0.2
          ):
    
    best_solution_overall = None
    objective_value_of_best_solution_overall = float('inf')

    iteration_count = 0
    iterations_without_improvement = 0
    start_time = time.time()

    valid_criteria = ["max_iterations", "max_time", "no_improvement"]
    if stopping_criterion not in valid_criteria:
        raise ValueError(f"Critério de parada '{stopping_criterion}' inválido. Use: {valid_criteria}")

    while True:
        iteration_count += 1
        #print(f"Iteração {iteration_count}...")

        current_solution = ConstructiveAlgorithm(solutions, alpha)

        if current_solution is None:
            #print("Nenhuma solução válida gerada.")
            if stopping_criterion == "no_improvement" and best_solution_overall is not None:
                 iterations_without_improvement += 1
        else:
            current_solution = BestImprovement(current_solution, neighbourhood)
            
            if current_solution is not None:
                current_objective_value = current_solution.objective_value()
                #print(f"Solução atual tem valor objetivo: {current_objective_value}")

                if current_objective_value < objective_value_of_best_solution_overall:
                    best_solution_overall = current_solution
                    objective_value_of_best_solution_overall = current_objective_value
                    iterations_without_improvement = 0
                    #print("Nova melhor solução encontrada!")

                elif stopping_criterion == "no_improvement":
                    iterations_without_improvement += 1
            elif stopping_criterion == "no_improvement" and best_solution_overall is not None:
                iterations_without_improvement += 1

        # Critérios de parada
        stop = False
        if stopping_criterion == "max_iterations" and iteration_count >= criterion_value: stop = True
        elif stopping_criterion == "max_time" and (time.time() - start_time) >= criterion_value: stop = True
        elif stopping_criterion == "no_improvement" and iterations_without_improvement >= criterion_value: stop = True
        
        if stop:
            #print(f"Criério de parada atingido após {iteration_count} iterações.")
            break
            
    return best_solution_overall