#include "LifeCondition.h"
#include "set"
#include "unordered_map"

LifeCondition::LifeCondition(unsigned size, Special special) : size_(size) {
    if (special == Special::planer || special == Special::zeros) {
        for (int i = 0; i < size_; ++i) {
            state_.push_back({});
            for (int j = 0; j < size_; ++j) {
                state_[i].push_back(false);
            }
        }
        if (special == Special::planer) {
            state_[2][0] = true;
            state_[2][1] = true;
            state_[2][2] = true;
            state_[1][2] = true;
            state_[0][1] = true;
        }
    } else if (special == Special::random) {
        for (int i = 0; i < size_; ++i) {
            state_.push_back({});
            for (int j = 0; j < size_; ++j) {
                state_[i].push_back(rand() % 2);
            }
        }
    }
}

LifeCondition::LifeCondition(unsigned size, const std::vector<std::vector<bool>>& state) {
    if (state.size() != size) {
        throw std::exception();
    }
    for (const auto& row : state) {
        if (row.size() != size) {
            throw std::exception();
        }
    }
    size_ = size;
    state_ = state;
}

bool LifeCondition::operator== (const LifeCondition& other) const {
    if (size_ != other.size_) {
        return false;
    }
    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            if (state_[i][j] != other.state_[i][j]) {
                return false;
            }
        }
    }
    return true;
}

LifeCondition LifeCondition::SkipConditions(int step_num) const {
    auto cur = *this;
    for (int step = 0; step < step_num; ++step) {
        auto cur_copy = LifeCondition(cur.GetSize(), Special::zeros);
        for (int i = 0; i < size_; ++i) {
            int first_row = 0;
            int second_row = 0;
            if (cur.state_[i][0]) {
                second_row += 1;
            }
            if (i + 1 < size_ && cur.state_[i + 1][0]) {
                second_row += 1;
            }
            if (i - 1 >= 0 && cur.state_[i - 1][0]) {
                second_row += 1;
            }
            int third_row;
            for (int j = 0; j < size_; ++j) {
                third_row = 0;
                if (j + 1 < size_) {
                    if (cur.state_[i][j + 1]) {
                        third_row += 1;
                    }
                    if (i + 1 < size_ && cur.state_[i + 1][j + 1]) {
                        third_row += 1;
                    }
                    if (i - 1 >= 0 && cur.state_[i - 1][j + 1]) {
                        third_row += 1;
                    }
                }
                int alive_neighbours = first_row + second_row + third_row - (cur.state_[i][j] ? 1 : 0);
                if (alive_neighbours == 3) {
                    cur_copy.state_[i][j] = true;
                } else if (alive_neighbours == 2 && cur.state_[i][j]) {
                    cur_copy.state_[i][j] = true;
                }
                first_row = second_row;
                second_row = third_row;
            }
        }
        cur = cur_copy;
    }
    return cur;
}

bool LifeCondition::IsStationary() const {
    return SkipConditions(1) == *this;
}

std::string LifeCondition::ToString() const {
    std::string res = "";
    for (int i = 0; i < size_; ++i) {
        for (int j = 0; j < size_; ++j) {
            res += (state_[i][j] ? 'X' : '-');
        }
        res += '\n';
    }
    return res;
}

double LifeCondition::OptValue() const {
    if (!counted_alive_) {
        CountOptValue();
    }
    return alive_in_100_iter_ + (IsValid() ? 0 : size_ * size_);
}

bool LifeCondition::IsValid() const {
    if (!counted_alive_) {
        CountOptValue();
    }
    return !static_in_100_iter_;
}

void LifeCondition::CountOptValue() const {
    auto next = SkipConditions(100);
    auto count_alive = 0;
    for (const auto& row : next.GetState()) {
        for (const auto& cell : row) {
            if (cell) {
                count_alive += 1;
            }
        }
    }
    alive_in_100_iter_ = count_alive;
    static_in_100_iter_ = next.IsStationary();
    counted_alive_ = true;
}