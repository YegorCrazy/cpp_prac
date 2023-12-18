#include "Breed.h"
#include "LifeCondition.h"

std::vector<LifeCondition> OnePointBreed::Breed(LifeCondition first, LifeCondition second) {
    if (first.GetSize() != second.GetSize()) {
        throw std::exception();
    }
    unsigned cross_point = rand() % (first.GetSize() * first.GetSize() + 1);
    if (cross_point == 0 || cross_point == first.GetSize() * first.GetSize()) {
        return {first, second};
    }
    return {BreedOne(first, second, cross_point), BreedOne(second, first, cross_point)};
}

LifeCondition OnePointBreed::BreedOne(LifeCondition first, LifeCondition second, unsigned start) {
    // биты с индексом start (с нуля) и больше берутся из второго родителя, остальные из первого
    if (first.GetSize() != second.GetSize()) {
        throw std::exception();
    }
    std::vector<std::vector<bool>> res;
    auto size = first.GetSize();
    for (int i = 0; i < size; ++i) {
        if (start > (i + 1) * size - 1) {
            res.push_back(first.GetState()[i]);
        } else if (start < (i) * size) {
            res.push_back(second.GetState()[i]);
        } else {
            res.push_back({});
            for (int j = 0; j < size; ++j) {
                if (start > i * size + j) {
                    res[i].push_back(first.GetState()[i][j]);
                } else if (start <= i * size + j) {
                    res[i].push_back(second.GetState()[i][j]);
                }
            }
        }
    }
    return LifeCondition(size, res);
}