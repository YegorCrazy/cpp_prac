#include <iostream>
#include <fstream>
#include "generate.h"

int main() {
    std::srand(time(NULL));
    unsigned tasks_num, min_len, max_len;
    std::cin >> tasks_num >> min_len >> max_len;
    std::ofstream out_file("out.csv", std::ios_base::out | std::ios_base::trunc);
    out_file << tasks_num << "\n";
    auto tasks = generate_tasks(tasks_num, min_len, max_len);
    for (const auto& task_len : tasks) {
        out_file << task_len << "\n";
    }
    out_file.close();
    return 0;
}