#pragma once

#include <vector>
#include "LifeCondition.h"
#include <memory>

class TSelection {
public:
    virtual std::shared_ptr<std::vector<LifeCondition>> Select(std::vector<LifeCondition>& population, unsigned get_num) = 0;
};

class TournamentSelection : public TSelection {
public:
    TournamentSelection(unsigned tournament_players) : tournament_players_(tournament_players) {};
    std::shared_ptr<std::vector<LifeCondition>> Select(std::vector<LifeCondition>& population, unsigned get_num) override;
private:
    unsigned tournament_players_;
};