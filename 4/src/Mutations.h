#pragma once

#include "LifeCondition.h"
#include <cstdlib>

class TMutation {
public:
    virtual LifeCondition Mutate(const LifeCondition& cond) = 0;
};

class RandomMutation : public TMutation {
public:
    RandomMutation(double prob) : prob_(prob) {}
    LifeCondition Mutate(const LifeCondition& cond) override {
        auto state = cond.GetState();
        for (auto& row : state) {
            for (auto cell : row) {
                if (rand() / (double)RAND_MAX < prob_) {
                    cell = !cell;
                }
            }
        }
        return LifeCondition(state.size(), state);
    }
private:
    double prob_;
};