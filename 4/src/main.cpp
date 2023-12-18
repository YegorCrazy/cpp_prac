#include <iostream>
#include <memory>
#include <ncurses.h>
#include <system_error>
#include <thread>
#include <vector>
#include <chrono>

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
    unsigned LifeSize = 50;
    auto test = LifeCondition(LifeSize);
    auto now = std::chrono::system_clock::now();
    for (int i = 0; i < 100; ++i) {
        test.SkipConditions(100);
    }
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - now).count() << std::endl;

    // unsigned population_size = 100;
    // unsigned LifeSize = 50;
    // std::vector<LifeCondition> start_population;
    // for (int i = 0; i < population_size; ++i) {
    //     start_population.push_back(LifeCondition(LifeSize));
    // }
    // MainCycleParams param{start_population, std::make_shared<TournamentSelection>(2), 
    //     std::make_shared<OnePointBreed>(), std::make_shared<RandomMutation>(0.5),
    //     0.8, population_size};
    // auto res_cond = MainCycle(param);
    // std::cout << res_cond.ToString() << std::endl << 
    //     res_cond.SkipConditions(100).ToString() << std::endl <<
    //     res_cond.SkipConditions(100).IsStationary() << std::endl;

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