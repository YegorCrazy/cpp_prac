#pragma once

#include <vector>
#include <string>

enum class Special {
    planer,
    zeros,
    random,
};

class LifeCondition {
public:
    LifeCondition(unsigned size, Special special = Special::random);
    LifeCondition(unsigned size, const std::vector<std::vector<bool>>& state);
    bool operator== (const LifeCondition& other) const;
    LifeCondition SkipConditions(int step_num) const;
    bool IsStationary() const;
    std::string ToString() const;
    std::vector<std::vector<bool>> GetState() const {
        return state_;
    }
    unsigned GetSize() const {
        return size_;
    }
    double OptValue() const;
    bool IsValid() const;
private:
    void CountOptValue() const ;
    unsigned size_;
    std::vector<std::vector<bool>> state_;
    mutable unsigned alive_in_100_iter_ = 0;
    mutable bool static_in_100_iter_ = 0;
    mutable bool counted_alive_ = false;
};