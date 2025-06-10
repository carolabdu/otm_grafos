import random
import time
from concurrent.futures import ProcessPoolExecutor, wait, FIRST_COMPLETED
from utils import ConstructiveAlgorithm, BestImprovement
from classes_solucao import KPFSolution
import secrets

def one_grasp_iteration(solutions, alpha, seed):
    random.seed(seed)
    sol = ConstructiveAlgorithm(solutions, alpha)
    if sol is None:
        return None
    return BestImprovement(sol)

def parallel_grasp(
    stopping_criterion: str,
    criterion_value: float,
    solutions,
    alpha: float = 0.2,
    n_workers: int = 4):
    valid = {"max_iterations", "max_time", "no_improvement"}
    if stopping_criterion not in valid:
        raise ValueError(f"Critério inválido: {valid}")

    best_sol = None
    best_val = -float('inf')
    iter_count = 0
    no_impr = 0
    start = time.time()

    def should_stop():
        if stopping_criterion == "max_iterations":
            return iter_count >= criterion_value
        if stopping_criterion == "max_time":
            return (time.time() - start) >= criterion_value
        return no_impr >= criterion_value

    with ProcessPoolExecutor(max_workers=n_workers) as exe:
        futures = set()
        for i in range(n_workers):
            seed = time.time_ns() + i
            futures.add(exe.submit(one_grasp_iteration, solutions, alpha, seed))

        while futures and not should_stop():
            done, _ = wait(futures, timeout=0.1, return_when=FIRST_COMPLETED)
            if not done:
                continue
            for fut in done:
                futures.remove(fut)
                sol = fut.result()
                iter_count += 1

                if sol is not None:
                    val = sol.objective_value()
                    if val > best_val:
                        best_val, best_sol = val, sol
                        no_impr = 0
                    elif stopping_criterion == "no_improvement":
                        no_impr += 1
                else:
                    if stopping_criterion == "no_improvement" and best_sol is not None:
                        no_impr += 1

                if not should_stop():
                    new_seed = time.time_ns() ^ secrets.randbits(64)
                    futures.add(exe.submit(one_grasp_iteration, solutions, alpha, new_seed))

    return best_sol
