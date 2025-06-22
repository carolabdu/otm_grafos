#include "gurobi_c++.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <omp.h>

namespace fs = std::filesystem;
using namespace std;

// Estrutura para armazenar as métricas de resultado de uma única instância
struct ResultData {
    string instanceName;
    int status; // Mantido para lógica interna, mas não será salvo no CSV
    double objectiveValue = 0.0; // Incumbent
    double bestBound = 0.0;      // BestBd
    double mipGap = 0.0;         // Gap
    double solveTime = 0.0;
    double nodeCount = 0.0;
};

// Leitura da instância no formato de arquivo especificado
void readInstance(const string& filename,
                  int& n, int& l, int& b,
                  vector<int>& profits,
                  vector<int>& weights,
                  vector<int>& allowances,
                  vector<int>& penalties,
                  vector<vector<int>>& penaltySets) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Nao foi possivel abrir o arquivo: " + filename);
    }

    file >> n >> l >> b;

    profits.resize(n);
    for (int i = 0; i < n; ++i) file >> profits[i];

    weights.resize(n);
    for (int i = 0; i < n; ++i) file >> weights[i];

    allowances.resize(l);
    penalties.resize(l);
    penaltySets.resize(l);

    for (int i = 0; i < l; ++i) {
        int hi, di, ci;
        file >> hi >> di >> ci;

        allowances[i] = hi;
        penalties[i] = di;

        penaltySets[i].resize(ci);
        for (int j = 0; j < ci; ++j)
            file >> penaltySets[i][j];
    }

    file.close();
}

// A função agora aceita uma referência para um struct ResultData para salvar as métricas
void solveInstance(const string& filename, ResultData& result) {
    int thread_id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();
    cout << "Thread " << thread_id << " of " << num_threads << " is solving instance: " << filename << endl;

    result.instanceName = filename; // Salva o nome da instância

    try {
        int n, l, b, k;
        vector<int> p, w, h, d;
        vector<vector<int>> C;

        readInstance(filename, n, l, b, p, w, h, d, C);

        GRBEnv env = GRBEnv(true);
        
        // *** ALTERAÇÃO PRINCIPAL: Cria um caminho de log que espelha a estrutura de pastas da instância ***
        fs::path log_file_path = "log";
        log_file_path /= filename; // Anexa o caminho completo (ex: "log/instances/scenario1/...")
        log_file_path += ".log";   // Adiciona a extensão .log no final

        if (log_file_path.find("scenario1") != std::string::npos || log_file_path.find("scenario2") != std::string::npos){
        switch (n){
            case (300):
                k = n/15;
                break;
            case (500):
                k = n/25;
                break;
            case (700):
                k = n/35;
                break;
            case (800):
                k = std::round(n/45);
                break;
            case (1000):
                k = std::round(n/55);
                break;
        }
    } else {
        k = std::round(nI/15);
    }

        // Garante que a estrutura de diretórios de destino exista
        // ex: cria "log/instances/scenario1/correlated_sc1/300/"
        fs::create_directories(log_file_path.parent_path());

        env.set(GRB_StringParam_LogFile, log_file_path.string());
        env.start();
        
        GRBModel model = GRBModel(env);
        model.set(GRB_DoubleParam_TimeLimit, 1800.0);
        model.set(GRB_IntParam_Threads, 1);

        vector<GRBVar> x(n);
        for (int j = 0; j < n; ++j)
            x[j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + to_string(j));

        vector<GRBVar> v(l);
        for (int i = 0; i < l; ++i)
            v[i] = model.addVar(0.0, C[i].size() - h[i], 0.0, GRB_INTEGER, "v_" + to_string(i));

        GRBLinExpr obj = 0;
        for (int j = 0; j < n; ++j) obj += p[j] * x[j];
        for (int i = 0; i < l; ++i) obj -= d[i] * v[i];
        model.setObjective(obj, GRB_MAXIMIZE);

        GRBLinExpr capacity = 0;
        for (int j = 0; j < n; ++j) capacity += w[j] * x[j];
        model.addConstr(capacity <= b, "capacity");

        GRBLinExpr totalViolations = 0;
        for (int i = 0; i < l; ++i) totalViolations += v[i];
        model.addConstr(totalViolations <= k, "total_violations");

        for (int i = 0; i < l; ++i) {
            GRBLinExpr sumCi = 0;
            for (int j : C[i]) sumCi += x[j];
            model.addConstr(sumCi - v[i] <= h[i], "penalty_set_" + to_string(i));
        }

        model.optimize();

        result.status = model.get(GRB_IntAttr_Status);
        result.solveTime = model.get(GRB_DoubleAttr_Runtime);
        
        if (result.status == GRB_OPTIMAL || result.status == GRB_SUBOPTIMAL || result.status == GRB_TIME_LIMIT || result.status == GRB_INTERRUPTED) {
             if (model.get(GRB_IntAttr_SolCount) > 0) {
                result.objectiveValue = model.get(GRB_DoubleAttr_ObjVal); // Incumbent
             }
             result.bestBound = model.get(GRB_DoubleAttr_ObjBound); // BestBd
             result.mipGap = model.get(GRB_DoubleAttr_MIPGap);     // Gap
             result.nodeCount = model.get(GRB_DoubleAttr_NodeCount);
            }
    } catch (GRBException& e) {
        cerr << "Erro Gurobi para a instancia " << filename << ": " << e.getMessage() << endl;
    } catch (exception& e) {
        cerr << "Erro para a instancia " << filename << ": " << e.what() << endl;
    }
}

int main() {
    // As instâncias são organizadas em pastas dependendo do cenário e da correlação de dados 
    string base_directory = "instances";
    vector<string> instance_files;

    try {
        // A linha fs::create_directory("log") não é mais necessária aqui,
        // pois a função solveInstance agora cuida da criação das subpastas.
        for (const auto& entry : fs::recursive_directory_iterator(base_directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                string filename_str = entry.path().filename().string();
                if (filename_str.rfind("kpfs_", 0) == 0 && entry.path().extension() == ".txt") {
                    instance_files.push_back(entry.path().string());
                }
            }
        }
    } catch (fs::filesystem_error& e) {
        cerr << "Erro ao acessar o diretorio: " << e.what() << endl;
        return 1;
    }

    vector<ResultData> all_results(instance_files.size());

    #pragma omp parallel for
    for (int i = 0; i < instance_files.size(); ++i) {
        solveInstance(instance_files[i], all_results[i]);
    }

    cout << "Todas as instancias foram processadas. Gerando arquivo CSV..." << endl;
    ofstream csv_file("resultados_solver.csv");
    if (!csv_file.is_open()) {
        cerr << "Nao foi possivel criar o arquivo CSV." << endl;
        return 1;
    }

    csv_file << "Instancia,Incumbent,BestBd,Gap,TempoExecucao(s),NosExplorados\n";

    // Escreve os dados de cada instância
    for (const auto& result : all_results) {
        // *** ALTERAÇÃO 2: Escreve as novas colunas no CSV ***
        csv_file << result.instanceName << ","
                 << result.objectiveValue << ","
                 << result.bestBound << ","
                 << result.mipGap << ","
                 << result.solveTime << ","
                 << result.nodeCount << "\n";
    }
    csv_file.close();
    cout << "Arquivo 'resultados_solver.csv' gerado com sucesso." << endl;

    return 0;
}