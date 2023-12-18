#pragma once

#include "LifeCondition.h"

class TBreed {
public:
    virtual std::vector<LifeCondition> Breed(LifeCondition first, LifeCondition second) = 0;
};

class OnePointBreed : public TBreed {
public:
    std::vector<LifeCondition> Breed(LifeCondition first, LifeCondition second) override;
private:
    LifeCondition BreedOne(LifeCondition first, LifeCondition second, unsigned start);
};