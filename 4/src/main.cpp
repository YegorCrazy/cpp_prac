#include <iostream>
#include <memory>
#include <ncurses.h>
#include <thread>
#include <vector>
#include <chrono>
#include <cmath>
#include <fstream>

#include "LifeCondition.h"
#include "Mutations.h"
#include "Breed.h"
#include "Selection.h"
#include "MainCycle.h"

using namespace std::chrono_literals;

void draw_life(LifeCondition start) {
    initscr();
    while (true) {
        move(0, 0);
        printw("%s", start.ToString().c_str());
        refresh();
        start = start.SkipConditions(1);
        std::this_thread::sleep_for(500ms);
    }
}

int main() {
    srand(42);
    unsigned population_size = 100;
    unsigned LifeSize = 50;
    std::vector<std::thread> threads;
    for (int i = 1; i < 10; ++i) {
        threads.emplace_back(std::thread([=]() {
            double mutation_prob = 1.0 / (50 * 50) * (std::pow(1.5, i));
            double min_value = LifeSize * LifeSize;
            double max_value = 0;
            unsigned max_time = 0;
            LifeCondition best(LifeSize, Special::zeros);
            for (int j = 0; j < 10; ++j) {
                std::vector<LifeCondition> start_population;
                for (int k = 0; k < population_size; ++k) {
                    start_population.push_back(LifeCondition(LifeSize));
                }
                MainCycleParams param{start_population, std::make_shared<TournamentSelection>(2), 
                    std::make_shared<OnePointBreed>(), std::make_shared<RandomMutation>(mutation_prob),
                    0.8, population_size};
                auto now = std::chrono::system_clock::now();
                auto res_cond = MainCycle(param);
                if (!res_cond.IsValid()) {
                    std::cout << "WRONG SOLUTION!!!" << std::endl;
                }
                unsigned time = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now() - now).count();
                if (min_value > res_cond.OptValue()) {
                    best = res_cond;
                    min_value = res_cond.OptValue();
                }
                if (max_value < res_cond.OptValue()) {
                    max_value = res_cond.OptValue();
                }
                if (max_time < time) {
                    max_time = time;
                }
                std::string filename = "result/series_" + std::to_string(i) + "_run_" + std::to_string(j + 1) + "_sol";
                std::ofstream out_orig(filename + ".txt");
                out_orig << best.ToString();
                out_orig.close();
                std::ofstream out_end(filename + "_after100.txt");
                out_end << best.SkipConditions(100).ToString();
                out_end.close();
            }
            std::cout << i << ": " << min_value << ", " << max_value << ", " << max_time << std::endl;
        }));
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // unsigned LifeSize = 50;
    // auto test = LifeCondition(LifeSize);
    // auto now = std::chrono::system_clock::now();
    // for (int i = 0; i < 100; ++i) {
    //     test.SkipConditions(100);
    // }
    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count() << std::endl;

    // srand(42);
    // unsigned population_size = 100;
    // unsigned LifeSize = 50;
    // std::vector<LifeCondition> start_population;
    // for (int i = 0; i < population_size; ++i) {
    //     start_population.push_back(LifeCondition(LifeSize));
    // }
    // MainCycleParams param{start_population, std::make_shared<TournamentSelection>(2), 
    //     std::make_shared<OnePointBreed>(), std::make_shared<RandomMutation>(0.5),
    //     0.8, population_size};
    // auto now = std::chrono::system_clock::now();
    // auto res_cond = MainCycle(param);
    // std::cout << res_cond.ToString() << std::endl << 
    //     res_cond.SkipConditions(100).ToString() << std::endl <<
    //     res_cond.SkipConditions(100).IsStationary() << std::endl;
    // std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(
    //     std::chrono::system_clock::now() - now).count() << "ms" << std::endl;

    // std::vector<LifeCondition> test;
    // for (int i = 0; i < 10; ++i) {
    //     test.push_back(LifeCondition(6));
    // }
    // for (const auto& t : test) {
    //     std::cout << t.SkipConditions(100).ToString() << std::endl;
    // }
    // std::cout << "_______________________" << std::endl;

    // auto test2 = TournamentSelection(3).Select(test, 5);
    // for (const auto& t : test2) {
    //     std::cout << t.SkipConditions(100).ToString() << std::endl;
    // }

    // std::vector<std::vector<bool>> zeros_st, ones_st;
    // unsigned size = 6;
    // for (int i = 0; i < size; ++i) {
    //     zeros_st.push_back({});
    //     ones_st.push_back({});
    //     for (int j = 0; j < size; ++j) {
    //         zeros_st[i].push_back(false);
    //         ones_st[i].push_back(true);
    //     }
    // }
    // LifeCondition zeros = LifeCondition(size, zeros_st), ones = LifeCondition(size, ones_st);
    // for (const auto& br : OnePointBreed().Breed(zeros, ones)) {
    //     std::cout << br.ToString() << std::endl;
    // }

    // srand(42);
    // LifeCondition life(10);
    // auto mut = RandomMutation(0.5);
    // std::cout << life.ToString() << std::endl;
    // std::cout << mut.Mutate(life).ToString() << std::endl;

    // srand(42);
    // LifeCondition life(30, Special::planer);
    // draw_life(life);

    // for (int i = 0; i < 100; ++i) {
    //     std::cout << life.ToString() << std::endl;
    //     life = life.SkipConditions(1);
    // }
    // if (life.IsStationary()) {
    //     std::cout << "static" << std::endl;
    // }
    return 0;
}