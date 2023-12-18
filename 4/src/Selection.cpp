#include "Selection.h"

std::vector<LifeCondition> TournamentSelection::Select(const std::vector<LifeCondition>& population, unsigned get_num) {
    std::vector<LifeCondition> res;
    for (int i = 0; i < get_num; ++i) {
        LifeCondition best_player = population[rand() % population.size()];
        double min_value = best_player.OptValue();
        for (int j = 0; j < tournament_players_ - 1; ++j) {
            LifeCondition new_player = population[rand() % population.size()];
            double new_value = new_player.OptValue();
            if (new_value < min_value) {
                min_value = new_value;
                best_player = new_player;
            }
        }
        res.push_back(best_player);
    }
    return res;
}