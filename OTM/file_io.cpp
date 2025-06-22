#include "file_io.hpp"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cmath>

KPFSProblem loadKPFSInstance(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + file_path);
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        // Trim whitespace
        std::istringstream iss(line);
        std::string trimmed;
        if (!(iss >> std::ws).eof()) {
            // Non-empty line
            lines.push_back(line);
        }
    }
    file.close();

    int nI, nS, kS;
    {
        std::istringstream iss(lines[0]);
        iss >> nI >> nS >> kS;
    }

    // Profits
    std::vector<double> profits(nI);
    {
        std::istringstream iss(lines[1]);
        for (int i = 0; i < nI; ++i) iss >> profits[i];
    }

    // Weights
    std::vector<double> weights(nI);
    {
        std::istringstream iss(lines[2]);
        for (int i = 0; i < nI; ++i) iss >> weights[i];
    }

    // Build items
    std::vector<Item> items;
    items.reserve(nI);
    for (int i = 0; i < nI; ++i) {
        items.emplace_back(i, profits[i], weights[i]);
    }

    // Penalty sets
    std::vector<PenaltySet> penalty_sets;
    penalty_sets.reserve(nS);
    for (int si = 0; si < nS; ++si) {
        int header_idx = 3 + 2 * si;
        int list_idx   = header_idx + 1;

        int allowance, penalty, countItems;
        {
            std::istringstream iss(lines[header_idx]);
            iss >> allowance >> penalty >> countItems;
        }

        std::vector<std::size_t> indices;
        indices.reserve(countItems);
        {
            std::istringstream iss(lines[list_idx]);
            std::size_t id;
            for (int j = 0; j < countItems; ++j) {
                iss >> id;
                indices.push_back(id);
            }
        }

        penalty_sets.emplace_back(indices, allowance, static_cast<double>(penalty));
    }

    int k;
    // Create problem
    if (file_path.find("scenario1") != std::string::npos || file_path.find("scenario2") != std::string::npos){
        switch (nI){
            case (300):
                k = nI/15;
                break;
            case (500):
                k = nI/25;
                break;
            case (700):
                k = nI/35;
                break;
            case (800):
                k = std::round(nI/45);
                break;
            case (1000):
                k = std::round(nI/55);
                break;
        }
    }
    else{
        k = std::round(nI/15);
    }

    KPFSProblem problem(items, penalty_sets, static_cast<double>(kS), k);
    return problem;
}