#include "classes.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>

Solution::Solution(unsigned proc_num, const std::vector<unsigned>& works_len) :
    proc_num_(proc_num), works_len_(works_len)
{
    for (int i = 0; i < works_len_.size(); ++i) {
        timetable_[i] = 0;
    }
}

double Solution::score() const {
    std::vector<unsigned> lens;
    for (int i = 0; i < proc_num_; ++i) {
        lens.push_back(0);
    }
    for (const auto& [work_num, proc_num] : timetable_) {
        lens[proc_num] += works_len_[work_num];
    }
    return *std::max_element(lens.begin(), lens.end()) - *std::min_element(lens.begin(), lens.end());
}

void Solution::mutate () {
    unsigned work_num = rand() % works_len_.size();
    auto new_proc_num = rand() % proc_num_;
    while (new_proc_num == timetable_[work_num]) {
        new_proc_num = rand() % proc_num_;
    }
    timetable_[work_num] = new_proc_num;
}

ISolution* Solution::clone() const {
    return new Solution(*this);
}

TestSolution::TestSolution(unsigned proc_num, const std::vector<unsigned>& works_len) :
    Solution(proc_num, works_len)
{
    int next_proc = 0;
    for (int i = 0; i < works_len_.size(); ++i) {
        timetable_[i] = next_proc;
        next_proc += 1;
        next_proc %= proc_num;
    }
}

double TestSolution::score() const {
    return -Solution::score();
}

ISolution* TestSolution::clone() const {
    return new TestSolution(*this);
}

Mutator::Mutator(unsigned max_mutation_num) :
    max_mutation_num_(max_mutation_num) {}

void Mutator::mutate (ISolution* sol) const {
    unsigned mut_num = (rand() % max_mutation_num_) + 1;
    for (int i = 0; i < mut_num; ++i) {
        sol->mutate();
    }
}

double BoltzmannTemperatureDecrease::decrease(double current_temp, unsigned iter_num) const {
    return current_temp / std::log(1 + iter_num);
}

double CauchyTemperatureDecrease::decrease(double current_temp, unsigned iter_num) const {
    return current_temp / (1 + iter_num);
}

double LogTemperatureDecrease::decrease(double current_temp, unsigned iter_num) const {
    return current_temp * std::log(1 + iter_num) / (1 + iter_num);
}

MainCycle::MainCycle(const ISolution* start_sol, double start_temp, 
                     const IMutator* mutator, const ITemperatureDecrease* temp_decrease) :
    start_sol_(start_sol), start_temp_(start_temp), mutator_(mutator), temp_decrease_(temp_decrease) {}

ISolution* MainCycle::process () const {
    ISolution* cur_sol = start_sol_->clone();
    ISolution* best_sol = start_sol_->clone();
    double cur_temp = start_temp_;
    int iter_num = 1;
    int steps_without_improvement = 0;
    while (true) {
        for (int i = 0; i < 10; ++i) {
            auto new_sol = cur_sol->clone();
            mutator_->mutate(new_sol);
            if (new_sol->score() < best_sol->score()) {
                steps_without_improvement = 0;
                delete best_sol;
                best_sol = new_sol->clone();
            } else {
                steps_without_improvement += 1;
            }
            if (new_sol->score() < cur_sol->score()) {
                delete cur_sol;
                cur_sol = new_sol;
            } else {
                if (rand() / (double)RAND_MAX <= std::exp((cur_sol->score() - new_sol->score()) / cur_temp)) {
                    delete cur_sol;
                    cur_sol = new_sol;
                } else {
                    delete new_sol;
                }
            }
        }
        if (steps_without_improvement >= 100) {
            break;
        }
        cur_temp = temp_decrease_->decrease(cur_temp, iter_num);
        iter_num += 1;
    }
    delete cur_sol;
    return best_sol;
}