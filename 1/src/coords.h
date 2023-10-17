#pragma once

#include <stdexcept>

/**
    \brief Исключение слишком большого индекса

    Данное исключение возникает при попытке взять срез с индексами,
    не помещающимися в тип unsigned.
*/
class index_too_long_error : std::runtime_error {
public:
    index_too_long_error(std::string what, long long index) :
        std::runtime_error(what), index(index) {}
    long long index;
};

/// Класс координат среза
class Matrix_coords {
public:
    /// Конструктор
    Matrix_coords(long long r1, long long c1, long long r2, long long c2) {
        long long ind[4] = {r1, c1, r2, c2};
        for (int i = 0; i < 4; ++i) {
            long long x = ind[i];
            if (x == -1) {
                index[i] = 0;
                is_all[i] = true;
            } else {
                index[i] = x;
                if (index[i] != x) {
                    throw index_too_long_error("invalid index", r1);
                }
            }
        }
    }

    /// Конструктор по одной паре значений
    Matrix_coords(long long r1, long long c1) :
        Matrix_coords(r1, c1, r1, c1) {}

    /// Границы по строкам и столбцам (см. конструктор)
    unsigned index[4];
    /// Массив, поределяющий, какие из границ проходят до края матрицы
    bool is_all[4] = {false, false, false, false};
};

/// Координата среза строки
class Matrix_row_coord {
public:
    /// Конструктор
    Matrix_row_coord(unsigned row_num) :
        row_num(row_num) {}
    /// Номер строки
    unsigned row_num;
};

/// Координата среза строки
class Matrix_col_coord {
public:
    /// Конструктор
    Matrix_col_coord(unsigned col_num) :
        col_num(col_num) {}
    /// Номер столбца
    unsigned col_num;
};