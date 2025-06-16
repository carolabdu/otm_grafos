#include "file_io.hpp"
#include "grasp.hpp"      // GRASP, ParallelGRASP
#include "ils.hpp"        // ILS
#include "simulated_annealing.hpp" // SimulatedAnnealing
#include "tabu.hpp"       // TabuSearch
#include "utils.hpp"      // RandomSolutions, ConstructiveAlgorithm
#include "classes_base.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <numeric>
#include <sstream>
#include <cassert>
#include <stdexcept>
#include <cmath>

namespace fs = std::filesystem;
using Clock = std::chrono::high_resolution_clock;



int main() {
    // Configuration
    const std::string BASE_INSTANCE_PATH = "instances";
    const int N_RUNS = 5;
    const int INITIAL_SOLUTION_POOL_SIZE = 5;
    const std::string CSV_OUTPUT_FILE = "optimization_benchmark_full_results.csv";

    // Prepare CSV output (write header immediately)
    std::ofstream csv(CSV_OUTPUT_FILE);
    if (!csv.is_open()) {
        std::cerr << "Error: could not open CSV file for writing.\n";
        return 1;
    }
    csv << std::fixed << std::setprecision(4);
    csv << "Scenario,CorrelationType,Size,InstanceFile,Algorithm,Run,ObjectiveValue,ExecutionTime,Error\n";
    csv.flush();

    // Define algorithms
    struct Algo { std::string name; bool is_grasp; };
    std::vector<Algo> algos = {
        {"GRASP", true},
        {"ILS", false},
        {"SA", false},
        {"TabuSearch", false}
    };

    int total_processed = 0;
    // Iterate scenarios 1-4
    for (int s = 1; s <= 4; ++s) {
        std::string scenario = "scenario" + std::to_string(s);
        std::vector<std::string> corr_types = {
            "correlated_sc" + std::to_string(s),
            "fully_correlated_sc" + std::to_string(s),
            "not_correlated_sc" + std::to_string(s)
        };
        for (auto& corr : corr_types) {
            for (auto& size : {"300","500","700","800","1000"}) {
                for (int inst = 1; inst <= 10; ++inst) {
                    std::string filename = "kpfs_" + std::to_string(inst) + ".txt";
                    fs::path filepath = fs::path(BASE_INSTANCE_PATH) / scenario / corr / size / filename;

                    if (!fs::exists(filepath)) {
                        std::cout << "SKIPPING (not found): " << filepath << std::endl;
                        continue;
                    }
                    std::cout << "\n--- Processing Instance: " << filepath << " ---" << std::endl;
                    ++total_processed;

                    KPFSProblem problem({},{},0,0);
                    try {
                        problem = loadKPFSInstance(filepath.string());
                    } catch (const std::exception& e) {
                        std::cerr << "  ERROR loading instance: " << e.what() << std::endl;
                        // Log error row
                        csv << scenario << ',' << corr << ',' << size << ',' << filename
                            << ",N/A,0,NaN,0.0000,Failed to load instance" << '\n';
                        csv.flush();
                        continue;
                    }

                    // Generate initial pool for GRASP
                    auto initial_pool = RandomSolutions(problem, INITIAL_SOLUTION_POOL_SIZE);

                    // Per-algo accumulators for means
                    std::map<std::string, std::vector<double>> obj_vals;
                    std::map<std::string, std::vector<double>> times;

                    for (int run = 1; run <= N_RUNS; ++run) {
                        std::cout << "  Run " << run << "/" << N_RUNS << " for " << filename << std::endl;
                        for (auto& algo : algos) {
                            double objective = std::numeric_limits<double>::quiet_NaN();
                            double elapsed = 0.0;
                            std::string error = "";
                            
                            try {
                                auto t0 = Clock::now();
                                if (algo.name == "GRASP") {
                                    // example parameters
                                    auto best = GRASP(problem, 5, 100);
                                    objective = best.objectiveValue();
                                } else if (algo.name == "ILS") {
                                    auto best = ILS(problem, 100, 2, 0.2f);
                                    objective = best.objectiveValue();
                                } else if (algo.name == "SA") {
                                    auto best = Simulated_Annealing(problem, 100, 1e-3f, 100.0f, 0.95f);
                                    objective = best.objectiveValue();
                                } else if (algo.name == "TabuSearch") {
                                    auto best = TabuSearch(problem, 100, 10);
                                    objective = best.objectiveValue();
                                }
                                auto t1 = Clock::now();
                                elapsed = std::chrono::duration<double>(t1 - t0).count();

                                std::cout << "    " << algo.name
                                          << " - Obj: " << objective
                                          << ", Time: " << elapsed << "s" << std::endl;
                            } catch (const std::exception& e) {
                                error = e.what();
                                std::cerr << "    ERROR " << algo.name << ": " << error << std::endl;
                            }

                            // Write CSV row and flush
                            csv << scenario << ',' << corr << ',' << size << ',' << filename << ','
                                << algo.name << ',' << run << ','
                                << (std::isnan(objective) ? "NaN" : std::to_string(objective)) << ','
                                << elapsed << ','
                                << (error.empty() ? "" : error)
                                << '\n';
                            csv.flush();

                            // Accumulate for means
                            if (!std::isnan(objective)) {
                                obj_vals[algo.name].push_back(objective);
                                times[algo.name].push_back(elapsed);
                            }
                        }
                    }

                    // Print means per instance
                    std::cout << "\n  --- Mean Results for " << filename << " ---" << std::endl;
                    for (auto& algo : algos) {
                        auto& ov = obj_vals[algo.name];
                        auto& tm = times[algo.name];
                        double mean_obj = ov.empty() ? std::numeric_limits<double>::quiet_NaN()
                                                       : std::accumulate(ov.begin(), ov.end(), 0.0) / ov.size();
                        double mean_time = tm.empty() ? std::numeric_limits<double>::quiet_NaN()
                                                       : std::accumulate(tm.begin(), tm.end(), 0.0) / tm.size();
                        std::cout << "    " << algo.name
                                  << " Mean Obj: " << mean_obj
                                  << ", Mean Time: " << mean_time << "s" << std::endl;
                    }
                }
            }
        }
    }

    std::cout << "\n--- Benchmark Complete. Processed " << total_processed << " instances. ---" << std::endl;
    csv.close();
    return 0;
}
