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
    if (special == Special::zeros) {
        alive_in_100_iter_ = 0;
        static_in_100_iter_ = true;
    } else {
        CountOptValue();
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
    CountOptValue();
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
    LifeCondition cur = *this;
    for (int step = 0; step < step_num; ++step) {
        auto cur_copy = LifeCondition(cur.GetSize(), Special::zeros);
        std::unordered_map<unsigned, std::unordered_map<unsigned, unsigned>> with_2_or_3_neigh;
        std::set<std::pair<unsigned, unsigned>> to_process;
        for (int i = 0; i < size_; ++i) {
            for (int j = 0; j < size_; ++j) {
                if (cur.state_[i][j]) {
                    int alive_neighbours = 0;
                    for (int di = -1; di <= 1; ++di) {
                        for (int dj = -1; dj <= 1; ++dj) {
                            if (i + di >= 0 && i + di < size_ &&
                                j + dj >= 0 && j + dj < size_ &&
                                !(di == 0 && dj == 0))
                            {
                                to_process.insert({i + di, j + dj});
                                if (cur.state_[i + di][j + dj]) {
                                    alive_neighbours += 1;
                                }
                            }
                        }
                    }
                    if (alive_neighbours == 2 || alive_neighbours == 3) {
                        if (with_2_or_3_neigh.find(i) == with_2_or_3_neigh.end()) {
                            with_2_or_3_neigh[i] = {};
                        }
                        with_2_or_3_neigh[i][j] = alive_neighbours;
                    }
                }
            }
        }
        for (const auto& [i, j] : to_process) {
            int alive_neighbours = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    if (i + di >= 0 && i + di < size_ &&
                        j + dj >= 0 && j + dj < size_ &&
                        !(di == 0 && dj == 0))
                    {
                        if (cur.state_[i + di][j + dj]) {
                            alive_neighbours += 1;
                        }
                    }
                }
            }
            if (alive_neighbours == 2 || alive_neighbours == 3) {
                if (with_2_or_3_neigh.find(i) == with_2_or_3_neigh.end()) {
                    with_2_or_3_neigh[i] = {};
                }
                with_2_or_3_neigh[i][j] = alive_neighbours;
            }
        }
        for (const auto& [i, arr] : with_2_or_3_neigh) {
            for (const auto& [j, alive_num] : arr) {
                if (alive_num == 3) {
                    cur_copy.state_[i][j] = true;
                } else if (cur.state_[i][j]) {
                    cur_copy.state_[i][j] = true;
                }
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
    return alive_in_100_iter_ + (IsValid() ? 0 : size_ * size_);
}

bool LifeCondition::IsValid() const {
    return !static_in_100_iter_;
}

void LifeCondition::CountOptValue() {
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
}