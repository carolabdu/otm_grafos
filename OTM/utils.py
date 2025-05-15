import random
from classes_solucao import KPFSolution


# Configurado para função de minimização.
# Usar -f(x)
def ConstructiveAlgorithm(solutions, alpha = 0.2):

    h_min = float('inf')
    h_max = -float('inf')
    for solution in solutions:
        objValue = solution.objective_value()
        if (objValue < h_min):
            h_min = objValue
        elif (objValue > h_max):
            h_max = objValue
        else:
            continue

    lowerRange = h_min
    upperRange = h_max + alpha * (h_min - h_max)
    candidatesList = []
    for element in solutions:
        value = element.objective_value()
        if value >= lowerRange and value <= upperRange:
            candidatesList.append(element)

    return random.choice(candidatesList)
    

def BestImprovement(original_solution):
    n = len(original_solution.x)
    best_solution = original_solution.clone()
    best_obj_value = best_solution.objective_value()
    
    convergence = False
    while not convergence:
        convergence = True
        neighbourhood = []
        
        for i in range(n):
            neighbour = best_solution.clone()
            neighbour.toggle_item(i)  
            neighbourhood.append(neighbour)

        for current_solution in neighbourhood:
            current_obj_value = current_solution.objective_value()
            if current_obj_value > best_obj_value:
                convergence = False
                best_solution = current_solution
                best_obj_value = current_obj_value

    return best_solution
