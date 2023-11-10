#include <iostream>
#include "classes.h"

int main() {
    srand(time(NULL));
    Solution* start_sol = new Solution(3, {1, 2, 3, 1});
    std::cout << start_sol->score() << std::endl;
    Mutator mutator(3);
    BoltzmannTemperatureDecrease temp_decr;
    auto best_sol = MainCycle(start_sol, 100, &mutator, &temp_decr).process();
    for (const auto& [work, proc] : ((TestSolution*)best_sol)->get_timetable()) {
        std::cout << work << " " << proc << std::endl;
    }
    std::cout << best_sol->score() << std::endl;
    delete start_sol;
    delete best_sol;
    return 0;
}