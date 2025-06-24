# KPFS Solver
Este projeto implementa heurísticas e um resolvedor exato com Gurobi para o problema KPFS.

---

## Requisitos
- C++17
- G++ com OpenMP
- Gurobi 12.0.2 (com licença ativa)
- Make

---

## Compilação e Execução

### Heurísticas

```bash
make
./test_exec
```
Executa os benchmarks para as heurísticas ILS, GRASP, SA e Tabu.

### Gurobi

Edite o Makefile:
```makefile
GUROBI_HOME = ~/caminho/para/gurobi
GRB_LICENSE_FILE = /caminho/para/gurobi.lic
```

```bash
make gurobi
./gurobi_metrics
```
O executável percorre automaticamente as instâncias em instances/ e salva os resultados em CSV.

---

## Implementação de algoritmos:

- Algoritmo Construtivo e Best Improvement: utils.cpp
- Classes Item, PenaltySet, KPFSProblem, KPFSolution: classes_base.cpp
- Grasp: grasp.cpp
- React Grasp: react_grasp.cpp
- ILS: ils.cpp
- Simulated Annealing: simulated_annealing.cpp
- Tabu Search: tabu.cpp

## Resultados: 

Os resultados finais com as métricas de média, desvio padrão e melhor solução,  dos algoritmos e do artigo, estão no notebook: analysis.ipynb.

