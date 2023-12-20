#include "Selection.h"

std::shared_ptr<std::vector<LifeCondition>> TournamentSelection::Select(std::vector<LifeCondition>& population, unsigned get_num) {
    auto res = std::make_shared<std::vector<LifeCondition>>();
    res->reserve(get_num);
    for (int i = 0; i < get_num; ++i) {
        LifeCondition& best_player = population[rand() % population.size()];
        double min_value = best_player.OptValue();
        for (int j = 0; j < tournament_players_ - 1; ++j) {
            LifeCondition& new_player = population[rand() % population.size()];
            double new_value = new_player.OptValue();
            if (new_value < min_value) {
                min_value = new_value;
                best_player = new_player;
            }
        }
        res->push_back(best_player);
    }
    return res;
}