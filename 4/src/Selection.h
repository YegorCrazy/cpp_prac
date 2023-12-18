#pragma once

#include <vector>
#include "LifeCondition.h"

class TSelection {
public:
    virtual std::vector<LifeCondition> Select(const std::vector<LifeCondition>& population, unsigned get_num) = 0;
};

class TournamentSelection : public TSelection {
public:
    TournamentSelection(unsigned tournament_players) : tournament_players_(tournament_players) {};
    std::vector<LifeCondition> Select(const std::vector<LifeCondition>& population, unsigned get_num) override;
private:
    unsigned tournament_players_;
};