from typing import List
from pathlib import Path
from classes_base import Item, KPFSProblem, PenaltySet

def load_kpfs_instance(file_path: str) -> KPFSProblem:
    with open(file_path, 'r') as file:
        lines = file.readlines()

    # Remove quebras de linha e separa os valores
    lines = [line.strip() for line in lines if line.strip()]

    # ===== Linha 1: parâmetros principais =====
    nI, nS, kS = map(int, lines[0].split())

    # ===== Linha 2: lucros dos itens =====
    profits = list(map(int, lines[1].split()))
    assert len(profits) == nI

    # ===== Linha 3: pesos dos itens =====
    weights = list(map(int, lines[2].split()))
    assert len(weights) == nI

    # ===== Criação dos objetos Item =====
    items = [Item(index=i, profit=profits[i], weight=weights[i]) for i in range(nI)]

    # ===== Leitura dos conjuntos de penalidade =====
    penalty_sets: List[PenaltySet] = []
    for i in range(nS):
        header_idx = 3 + 2 * i
        list_idx = header_idx + 1

        nA_i, fC_i, nI_i = map(int, lines[header_idx].split())
        item_ids = list(map(int, lines[list_idx].split()))
        assert len(item_ids) == nI_i

        penalty_sets.append(PenaltySet(indices=item_ids, allowance=nA_i, penalty=fC_i))

    # ===== Criação do problema =====
    problem = KPFSProblem(items=items, penalty_sets=penalty_sets, capacity=kS, max_violations=nS)
    return problem
