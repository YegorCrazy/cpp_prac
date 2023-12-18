#pragma once

#include "LifeCondition.h"
#include "Selection.h"
#include "Breed.h"
#include "Mutations.h"
#include <memory>

struct MainCycleParams {
    std::vector<LifeCondition> start;
    std::shared_ptr<TSelection> selection;
    std::shared_ptr<TBreed> breed;
    std::shared_ptr<TMutation> mutation;
    double breed_prob;
    unsigned population_size;
};

double BestValue(std::vector<LifeCondition> population, LifeCondition& best);

LifeCondition MainCycle(const MainCycleParams& param);

