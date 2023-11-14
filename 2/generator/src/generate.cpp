#include <cstdlib>
#include "generate.h"

std::vector<unsigned> generate_tasks(unsigned tasks_num, unsigned min_len, unsigned max_len) {
    std::vector<unsigned> res;
    res.reserve(tasks_num);
    for (int i = 0; i < tasks_num; ++i) {
        res.push_back(std::rand() % (max_len - min_len + 1) + min_len);
    }
    return res;
}