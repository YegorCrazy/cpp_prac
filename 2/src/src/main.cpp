#include <cstdlib>
#include <iostream>
#include <fstream>
#include <chrono>
#include <memory>
#include <ostream>
#include <system_error>
#include "classes.h"
#include "generate.h"

std::vector<unsigned> read_csv(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<unsigned> res;
    unsigned works_num;
    file >> works_num;
    res.reserve(works_num);
    for (int i = 0; i < works_num; ++i) {
        unsigned len;
        file >> len;
        res.push_back(len);
    }
    return res;
}

int main() {
    int num_proc = 10;
    srand(time(NULL));
    auto works = read_csv("out.csv");
    Solution* start_sol = new Solution(num_proc, works);
    std::cout << "start score: " << start_sol->score() << std::endl;
    Mutator mutator(3);
    std::vector<std::shared_ptr<ITemperatureDecrease>> temp_decr_arr = {std::make_shared<BoltzmannTemperatureDecrease>()};
    for (const auto& temp_decr : temp_decr_arr) {
        int sum_secs = 0;
        unsigned sum_res = 0;
        int iter_num = 1;
        for (int i = 0; i < iter_num; ++i) {
            auto start = std::chrono::system_clock::now();
            auto best_sol = MainCycle(start_sol, 100, &mutator, temp_decr.get()).process();
            sum_secs += std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
            std::vector<unsigned> sum_works;
            for (int j = 0; j < num_proc; ++j) {
                sum_works.push_back(0);
            }
            for (const auto& [work, proc] : ((Solution*)best_sol)->get_timetable()) {
                sum_works[proc] += works[work];
            }
            for (const auto& summm : sum_works) {
                std::cout << summm << std::endl;
            }
            sum_res += best_sol->score();
            delete best_sol;
        }
        std::cout << "average score: " << (double)sum_res / iter_num << std::endl;
        std::cout << (double)sum_secs / iter_num << " secs" << std::endl;
    }
    delete start_sol;
    return 0;
}

// int main() {
//     srand(time(NULL));
//     //std::cout << "start score: " << start_sol->score() << std::endl;
//     Mutator mutator(3);
//     auto temp_decr = BoltzmannTemperatureDecrease();
//     std::cout << "[" << std::endl;
//     for (int proc_num = 2; proc_num <= 10; proc_num += 1) {
//         std::cout << "[";
//         for (int tasks_num = 1000; tasks_num <= 8000; tasks_num += 1000) {
//             if (tasks_num != 1000) {
//                 std::cout << ",";
//             }
//             Solution* start_sol = new Solution(proc_num, generate_tasks(tasks_num, 0, 100));
//             int sum_secs = 0;
//             int iter_num = 5;
//             for (int i = 0; i < iter_num; ++i) {
//                 auto start = std::chrono::system_clock::now();
//                 auto best_sol = MainCycle(start_sol, 100, &mutator, &temp_decr).process();
//                 sum_secs += std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - start).count();
//                 // for (const auto& [work, proc] : ((Solution*)best_sol)->get_timetable()) {
//                 //     std::cout << work << " " << proc << std::endl;
//                 // }
//                 delete best_sol;
//             }
//             std::cout << (double)sum_secs / iter_num;
//             std::flush(std::cout);
//             delete start_sol;
//         }
//         std::cout << "]," << std::endl;
//     }
//     std::cout << "]" << std::endl;
//     return 0;
// }