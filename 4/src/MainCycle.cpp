#include "MainCycle.h"

#include <iostream>
#include <chrono>

double BestValue(std::vector<LifeCondition> population, LifeCondition& best) {
    if (population.size() == 0) {
        return 0;
    }
    double min_value = population[0].GetSize() * population[0].GetSize();
    bool found_correct = false;
    for (const auto& sp : population) {
        if (sp.IsValid()) {
            found_correct = true;
            min_value = population[0].OptValue();
            best = population[0];
            break;
        }
    }
    if (!found_correct) {
        std::cout << "NO CORRECT SOLUTIONS FOUND" << std::endl;
    }
    for (const auto& sp : population) {
        if (!sp.IsValid()) {
            continue;
        }
        double new_value = sp.OptValue();
        if (new_value < min_value) {
            min_value = new_value;
            best = sp;
        }
    }
    return min_value;
}

LifeCondition MainCycle(const MainCycleParams& param) {
    if (param.start.size() == 0) {
        throw std::exception();
    }
    unsigned size = param.start[0].GetSize();
    for (const auto& sp : param.start) {
        if (sp.GetSize() != size) {
            throw std::exception();
        }
    }
    LifeCondition best_sol(size);
    double best_value = BestValue(param.start, best_sol);
    std::vector<LifeCondition> current_population = param.start;
    int without_gradeup = 0;
    while (without_gradeup < 50) {
        auto now = std::chrono::system_clock::now();
        auto selected = param.selection->Select(current_population, 
            current_population.size());
        std::vector<LifeCondition> new_population;
        while (new_population.size() < param.population_size) {
            LifeCondition first = selected[rand() % selected.size()];
            LifeCondition second = selected[rand() % selected.size()];
            double prob = rand() / (double)RAND_MAX;
            if (prob < param.breed_prob) {
                auto children = param.breed->Breed(first, second);
                // if (children[0].IsValid() && param.population_size > new_population.size()) {
                //     new_population.push_back(children[0]);
                // } 
                // if (children[1].IsValid() && param.population_size > new_population.size()) {
                //     new_population.push_back(children[1]);
                // }
                new_population.push_back(children[0]);
                new_population.push_back(children[1]);
            }
        }
        for (int i = 0; i < current_population.size(); ++i) {
            current_population[i] = param.mutation->Mutate(current_population[i]);
        }
        LifeCondition local_best(size);
        double local_min = BestValue(current_population, local_best);
        if (local_min < best_value) {
            best_value = local_min;
            best_sol = local_best;
            without_gradeup = 0;
            std::cout << "new best: " << best_value << std::endl;
        } else {
            without_gradeup += 1;
            std::cout << "no gradeup " << without_gradeup << std::endl;
        }
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - now).count() << "ms" << std::endl;
    }
    return best_sol;
}