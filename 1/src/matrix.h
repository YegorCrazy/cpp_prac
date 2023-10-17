#pragma once

#include "rational.h"
#include "coords.h"
#include <cctype>
#include <exception>
#include <vector>
#include <set>
#include <map>
#include <fstream>

/// Функция равенства нулю
inline bool is_zero(double eps) {
    return std::abs(eps) < 0.0000000001;
}

/// Функция преобразования строки в unsigned
inline unsigned int stoui(const std::string& s)
{
    unsigned long lresult = stoul(s, 0, 10);
    unsigned int result = lresult;
    if (result != lresult) throw std::out_of_range("too large index: " + s);
    return result;
}

/// Функция разбиения строки на подстроки по пробелу
inline std::vector<std::string> split(const std::string& s) {
    std::vector<std::string> res;
    std::string cur_str = "";
    for (const auto& c : s) {
        if (std::isspace(c) && !cur_str.empty()) {
            res.push_back(cur_str);
            cur_str.clear();
        }
        if (!std::isspace(c)) {
            cur_str.append(std::string(1, c));
        }
    }
    if (!cur_str.empty()) {
        res.push_back(cur_str);
    }
    return res;
}

template <typename T, template <class...> class M = std::map>
class Matrix;

template <class T, template <class...> class M>
class Matrix_proxy;

/**
    \brief Исключение неверного индекса

    Данный класс является исключением для ситуации, когда индекс
    элемента является некорректным.
*/
template <class T, template <class...> class M>
class invalid_index_error : std::runtime_error {
public:
    invalid_index_error(std::string what, Matrix<T, M> matr, std::pair<unsigned, unsigned> index) :
        std::runtime_error(what), index(index), matr(matr) {}
    Matrix<T, M> matr;
    std::pair<unsigned, unsigned> index;
};

/**
    \brief Исключение несовпадения размеров

    Данный класс является исключением для ситуации, когда размеры матриц
    не позволяют выполнить операцию сложения или вычитания над ними.
*/
template <class T, template <class...> class M, class T2, template <class...> class M2>
class size_differentiation_error : std::runtime_error {
public:
    size_differentiation_error(std::string what, Matrix<T, M> matr1, Matrix<T2, M2> matr2) :
        std::runtime_error(what), matr1(matr1), matr2(matr2) {}
    Matrix<T, M> matr1;
    Matrix<T2, M2> matr2;
};

/**
    \brief Исключение умножения

    Данный класс является исключением для ситуации, когда размеры матриц
    не позволяют выполнить операцию умножения над ними.
*/
template <class T, template <class...> class M, class T2, template <class...> class M2>
class multiplication_error : std::runtime_error {
public:
    multiplication_error(std::string what, Matrix<T, M> matr1, Matrix<T2, M2> matr2) :
        std::runtime_error(what), matr1(matr1), matr2(matr2) {}
    Matrix<T, M> matr1;
    Matrix<T2, M2> matr2;
};

/**
    \brief Исключение невалидного файла

    Данный класс является исключением для ситуации, когда файл неправильно
    описывает матрицу или типы элементов матриц не совпадают.
*/
class file_invalid_error : std::runtime_error {
public:
    file_invalid_error(std::string what, std::string file) :
        std::runtime_error(what), file(file) {}
    std::string file;
};

/**
    \brief Исключение неизвестного типа данных

    Данное исключение возникает при попытке записи матрицы с нестандартным
    типом элементов в файл (неизвестно, что писать в заголовке)
*/
template <class T, template <class...> class M>
class invalid_matrix_type_error : std::runtime_error {
public:
    invalid_matrix_type_error(std::string what, Matrix<T, M> matr) :
        std::runtime_error(what), matr(matr) {}
    Matrix<T, M> matr;
};

/**
    \brief Исключение неверного Matrix_coords

    Данный класс является исключением для ситуации, когда взятие
    среза невозможно из-за невалидных данных в Matrix_coords.
*/
template <class T, template <class...> class M>
class invalid_matrix_coords_error : std::runtime_error {
public:
    invalid_matrix_coords_error(std::string what, Matrix<T, M> matr, Matrix_coords coord) :
        std::runtime_error(what), matr(matr), coord(coord) {}
    Matrix<T, M> matr;
    Matrix_coords coord;
};

/**
    \brief Исключение неверного индекса

    Данный класс является исключением для ситуации, когда индекс
    элемента является некорректным (при использовании среза).
*/
template <class T, template <class...> class M>
class invalid_proxy_index_error : std::runtime_error {
public:
    invalid_proxy_index_error(std::string what, const Matrix_proxy<T, M>*proxy, std::pair<unsigned, unsigned> index) :
        std::runtime_error(what), index(index), proxy(proxy) {}
    const Matrix_proxy<T, M>*proxy;
    std::pair<unsigned, unsigned> index;
};

/**
    \brief Исключение удаленного родителя

    Данный класс является исключением для ситуации, когда при обращении
    к элементу среза родитель оказывается удален.
*/
class parent_deleted_error : std::runtime_error {
public:
    parent_deleted_error(std::string what) : std::runtime_error(what) {}
};

/**
    \brief Класс матриц

    Аргументы шаблона: тип элементов, тип контейнера
*/
template <typename T, template <class...> class M>
class Matrix {

public:

    /// Конструктор по размерам матрицы
    Matrix(unsigned rows_num, unsigned cols_num, double eps) :
        rows_num_(rows_num), cols_num_(cols_num), eps_(eps) {}

    /// Конструктор по элементам
    Matrix(const M<unsigned, M<unsigned, T>>& map, unsigned rows_num, unsigned cols_num, double eps) :
        rows_num_(rows_num), cols_num_(cols_num), eps_(eps) 
    {
        for (auto& [row_num, row] : map) {
            for (auto& [col_num, elem] : row) {
                if (row_num < 1 || row_num > rows_num) {
                    throw invalid_index_error("row num out of range", *this, {row_num, col_num});
                }
                if (col_num < 1 || col_num > cols_num) {
                    throw invalid_index_error("col num out of range", *this, {row_num, col_num});
                }
            }
        }
        map_ = map;
        delete_zeros();
    }

    /// Конструктор по срезу
    template <template <class...> class M2>
    Matrix(const Matrix_proxy<T, M2>& pr, double eps) : eps_(eps) {
        auto par = pr.get_parent();
        auto r = pr.get_row_index();
        auto c = pr.get_col_index();
        rows_num_ = r.second - r.first + 1;
        cols_num_ = c.second - c.first + 1;
        for (const auto& [row_num, row] : par->get_map()) {
            if (row_num >= r.first && row_num <= r.second) {
                for (const auto& [col_num, elem] : row) {
                    if (col_num >= c.first && col_num <= c.second) {
                        operator[](std::make_pair(row_num - r.first + 1, col_num - c.first + 1)) = elem;
                    }
                }
            }
        }
    }

    /// Деструктор
    ~Matrix() {
        for (const auto& pr : proxy_) {
            pr->parent_deleted();
        }
    }

    /// Конструктор копирования
    Matrix(const Matrix& other) : rows_num_(other.rows_num_), cols_num_(other.cols_num_), 
        eps_(other.eps_), map_(other.map_) {}

    /// Конструктор копирования по rvalue-ссылке
    Matrix(Matrix&& other) : rows_num_(other.rows_num_), cols_num_(other.cols_num_), 
        eps_(other.eps_), map_(other.map_) {}

    /// Оператор копирования
    Matrix& operator= (const Matrix& other) {
        for (const auto& pr : proxy_) {
            pr->parent_deleted();
        }
        proxy_.clear();
        rows_num_ = other.rows_num_;
        cols_num_ = other.cols_num_;
        eps_ = other.eps;
        map_ = other.map_;
    }

    /// Оператор копирования по rvalue-ссылке
    Matrix& operator= (Matrix&& other) {
        for (const auto& pr : proxy_) {
            pr->parent_deleted();
        }
        proxy_.clear();
        rows_num_ = other.rows_num_;
        cols_num_ = other.cols_num_;
        eps_ = other.eps;
        map_ = other.map_;
    }

    /// Создание единичной матрицы заданного размера
    static Matrix make_unary(unsigned rows_num, unsigned cols_num, double eps) {
        Matrix res(rows_num, cols_num, eps);
        for (int i = 1; i <= rows_num; ++i) {
            bool row_created = false;
            for (int j = 1; j <= cols_num; ++j) {
                if (is_zero(eps)) {
                    if (!row_created) {
                        res.map_[i] = {};
                        row_created = true;
                    }
                    res.map_[i][j] = 0;
                }
                if (i == j && eps < 1) {
                    if (!row_created) {
                        res.map_[i] = {};
                        row_created = true;
                    }
                    res.map_[i][j] = 1;
                }
            }
        }
        res.eps_ = eps;
        return res;
    }

    /// Создание нулевой матрицы заданного размера
    static Matrix make_zeros(unsigned rows_num, unsigned cols_num, double eps) {
        Matrix res(rows_num, cols_num, eps);
        if (is_zero(eps)) {
            for (int i = 1; i <= rows_num; ++i) {
                res.map_[i] = {};
                for (int j = 1; j <= cols_num; ++j) {
                    res.map_[i][j] = 0;
                }
            }
        }
        res.eps_ = eps;
        return res;
    }

    /// Создание матрицы заданного размера, заполненной единицами
    static Matrix make_ones(unsigned rows_num, unsigned cols_num, double eps) {
        Matrix res(rows_num, cols_num, eps);
        if (eps < 1) {
            for (int i = 1; i <= rows_num; ++i) {
                res.map_[i] = {};
                for (int j = 1; j <= cols_num; ++j) {
                    res.map_[i][j] = 1;
                }
            }
        }
        res.eps_ = eps;
        return res;
    }

    /// Оператор считывания матрицы из файла
    static Matrix from_file(std::string file_path, double eps) {
        std::ifstream in;
        in.open(file_path);
        std::string s;
        std::getline(in, s);
        auto w = split(s);
        while (w.empty() || w[0][0] == '#') {
            std::getline(in, s);
            w = split(s);
        }
        if (w.size() < 4) {
            throw file_invalid_error("file header broken", file_path);
        }
        if (w[0] != "matrix") {
            throw file_invalid_error("no matrix in file", file_path);
        }
        if (w[1] == "rational") {
            if (!std::is_same_v<T, RationalNumber<int>>) {
                throw file_invalid_error("invalid type", file_path);
            }
        } else if (w[1] == "integer") {
            if (!std::is_same_v<T, int>) {
                throw file_invalid_error("invalid type", file_path);
            }
        } else if (w[1] == "float") {
            if (!std::is_same_v<T, double>) {
                throw file_invalid_error("invalid type", file_path);
            }
        } else {
            throw file_invalid_error("unknown type", file_path);
        }
        int rows_num, cols_num;
        try {
            rows_num = stoui(w[2]);
            cols_num = stoui(w[3]);
        } catch (std::exception& ex) {
            throw file_invalid_error("invalid matrix size", file_path);
        }
        Matrix res(rows_num, cols_num, eps);
        while (std::getline(in, s)) {
            w = split(s);
            if (w.empty() || w[0][0] == '#') {
                continue;
            }
            if (w.size() < 3) {
                throw file_invalid_error("invalid line: " + s, file_path);
            }
            int row_num, col_num;
            try {
                row_num = stoui(w[0]);
                col_num = stoui(w[1]);
            } catch (std::exception& ex) {
                throw file_invalid_error("invalid index: " + s, file_path);
            }
            if (row_num < 1 || row_num > rows_num) {
                throw file_invalid_error("invalid row index: " + s, file_path);
            }
            if (col_num < 1 || col_num > cols_num) {
                throw file_invalid_error("invalid col index: " + s, file_path);
            }
            if (std::is_same_v<T, RationalNumber<int>>) {
                if (w[2][0] != '<' || w[2][w[2].size() - 1] != '>') {
                    throw file_invalid_error("invalid rational: " + s, file_path);
                }
                try {
                    res[std::make_pair(row_num, col_num)] = T(RationalNumber(w[2].substr(1, w[2].size() - 2).c_str()));
                } catch (std::exception& ex) {
                    throw file_invalid_error("invalid rational: " + s, file_path);
                }
            } else if (std::is_same_v<T, int>) {
                try {
                    res[std::make_pair(row_num, col_num)] = std::stoi(w[2]);
                } catch (std::exception& ex) {
                    throw file_invalid_error("invalid integer: " + s, file_path);
                }
            } else if (std::is_same_v<T, double>) {
                try {
                    res[std::make_pair(row_num, col_num)] = std::stod(w[2]);
                } catch (std::exception& ex) {
                    throw file_invalid_error("invalid float: " + s, file_path);
                }
            }
        }
        if (is_zero(eps)) {
            for (unsigned i = 1; i <= rows_num; ++i) {
                for (unsigned j = 1; j <= cols_num; ++j) {
                    // при обращении создается нулевой элемент, но потом он не удалится,
                    // и матрица будет полной
                    res[std::make_pair(i, j)];
                }
            }
        }
        return res;
    }

    /// Оператор превращения матрицы в строку
    std::string to_file_string () const {
        using std::to_string;
        std::string res = "matrix ";
        if (std::is_same_v<T, RationalNumber<int>>) {
            res += "rational ";
        } else if (std::is_same_v<T, int>) {
            res += "integer ";
        } else if (std::is_same_v<T, double>) {
            res += "float ";
        } else {
            throw invalid_matrix_type_error("unknown type name", *this);
        }
        res += std::to_string(rows_num_) + " " + std::to_string(cols_num_) + "\n";
        for (auto& [row_num, row] : map_) {
            for (auto& [col_num, elem_t] : row) {
                T elem = elem_t;
                res += std::to_string(row_num) + " " + std::to_string(col_num) + " ";
                std::string to_add;
                if (std::is_same_v<T, RationalNumber<int>>) {
                    to_add = "<" + to_string(elem) + ">";
                } else {
                    to_add = to_string(elem);
                }
                res += to_add + "\n";
            }
        }
        return res;
    }

    /// Оператор транспонирования
    Matrix operator~ () const {
        Matrix res(cols_num_, rows_num_, eps_);
        for (const auto& [row_num, row] : map_) {
            for (const auto& [col_num, num] : row) {
                res[std::make_pair(col_num, row_num)] = num;
            }
        }
        return res;
    }

    /// Унарный минус
    Matrix operator- () const {
        Matrix res = *this;
        for (const auto& [row_num, row] : res.map_) {
            for (const auto& [col_num, num] : row) {
                res.map_[row_num][col_num] = -res.map_[row_num][col_num];
            }
        }
        return res;
    }

    /// Оператор сложения
    template<class T2, template <class...> class M2>
    Matrix operator+= (const Matrix<T2, M2>& other) {
        if (rows_num_ != other.get_rows_num() || cols_num_ != other.get_cols_num()) {
            throw size_differentiation_error("size differs", *this, other);
        }
        for (const auto& [row_num, row] : other.get_map()) {
            for (const auto& [col_num, num] : row) {
                operator[](std::make_pair(row_num, col_num)) += num;
            }
        }
        delete_zeros();
        return *this;
    }

    /// Оператор вычитания
    template<class T2, template <class...> class M2>
    Matrix operator-= (const Matrix<T2, M2>& other) {
        return operator+=(-other);
    }

    /// Оператор умножения
    template<class T2, template <class...> class M2>
    Matrix operator*= (const Matrix<T2, M2>& other) {
        if (cols_num_ != other.get_rows_num()) {
            throw multiplication_error("multiplication failed", *this, other);
        }
        Matrix res(rows_num_, other.get_cols_num(), eps_);
        for (int i = 1; i <= rows_num_; ++i) {
            for (int j = 1; j <= other.get_cols_num(); ++j) {
                T cur_res = 0;
                for (int s = 1; s <= cols_num_; ++s) {
                    cur_res += (operator()(i, s) * other(s, j));
                }
                res[std::make_pair(i, j)] = cur_res;
            }
        }
        map_ = res.map_;
        rows_num_ = res.rows_num_;
        cols_num_ = res.cols_num_;
        return *this;
    }

    /// Оператор умножения на число
    Matrix operator*= (double k) {
        for (auto& [row_num, row] : map_) {
            for (auto& [col_num, elem] : row) {
                operator[](std::make_pair(row_num, col_num)) *= k;
            }
        }
        return *this;
    }

    /// Оператор сложения
    template<class T2, template <class...> class M2>
    friend Matrix operator+ (Matrix lhs, const Matrix<T2, M2>& rhs) {
        return lhs += rhs;
    }

    /// Оператор вычитания
    template<class T2, template <class...> class M2>
    friend Matrix operator- (Matrix lhs, const Matrix<T2, M2>& rhs) {
        return lhs -= rhs;
    }

    /// Оператор умножения
    template<class T2, template <class...> class M2>
    friend Matrix operator* (Matrix lhs, const Matrix<T2, M2>& rhs) {
        return lhs *= rhs;
    }

    /// Оператор умножения на число
    friend Matrix operator* (Matrix lhs, double rhs) {
        return lhs *= rhs;
    }

    /// Оператор получения контейнера элементов
    const auto& get_map () const {
        delete_zeros();
        return map_;
    }

    /// Оператор получения минимального модуля элемента
    double get_eps () const {
        return eps_;
    }

    /// Оператор получения количества строк
    unsigned get_rows_num () const {
        return rows_num_;
    }

    /// Оператор получения количества столбцов
    unsigned get_cols_num () const {
        return cols_num_;
    }

    /// Оператор доступа к элементу
    T& operator[] (std::pair<unsigned, unsigned> coord) {
        if (coord.first < 1 || coord.first > rows_num_) {
            throw invalid_index_error("invalid index", *this, coord);
        }
        if (coord.second < 1 || coord.second > cols_num_) {
            throw invalid_index_error("invalid index", *this, coord);
        }
        delete_zeros();
        if (map_.find(coord.first) == map_.end()) {
            map_[coord.first] = {};
        }
        if (map_[coord.first].find(coord.second) == map_[coord.first].end()) {
            map_[coord.first][coord.second] = 0;
        }
        return map_[coord.first][coord.second];
    }

    /// Создание среза по Matrix_coords
    Matrix_proxy<T, M>* operator[] (const Matrix_coords& c) {
        unsigned start_row = (c.is_all[0] ? 1 : c.index[0]);
        unsigned end_row = (c.is_all[2] ? rows_num_ : c.index[2]);
        unsigned start_col = (c.is_all[1] ? 1 : c.index[1]);
        unsigned end_col = (c.is_all[3] ? cols_num_ : c.index[3]);
        if (start_row > end_row || start_col > end_col) {
            throw invalid_matrix_coords_error("start row or col are greater than end", *this, c);
        }
        if (start_row < 1 || end_row > rows_num_ || start_col < 1 || end_col > cols_num_) {
            throw invalid_matrix_coords_error("index out of range", *this, c);
        }
        auto res = new Matrix_proxy(this, start_row, end_row, start_col, end_col);
        proxy_.insert(res);
        return res;
    }

    /// Создание среза по Matrix_row_coord
    Matrix_proxy<T, M>* operator[] (const Matrix_row_coord& c) {
        return operator[](Matrix_coords(c.row_num, -1, c.row_num, -1));
    }

    /// Создание среза по Matrix_col_coord
    Matrix_proxy<T, M>* operator[] (const Matrix_col_coord& c) {
        return operator[](Matrix_coords(-1, c.col_num, -1, c.col_num));
    }

    /// Оператор удаления среза
    void detach_proxy(const Matrix_proxy<T, M>* pr) const {
        proxy_.erase(pr);
    }

    /// Оператор константного доступа к элементу
    T operator() (unsigned row_num, unsigned col_num) const {
        if (row_num < 1 || row_num > rows_num_) {
            throw invalid_index_error("invalid index", *this, {row_num, col_num});
        }
        if (col_num < 1 || col_num > cols_num_) {
            throw invalid_index_error("invalid index", *this, {row_num, col_num});
        }
        delete_zeros();
        if (map_.find(row_num) == map_.end()) {
            return 0;
        }
        if (map_[row_num].find(col_num) == map_[row_num].end()) {
            return 0;
        }
        return map_[row_num][col_num];
    }

    /// Оператор удаления нулевых элементов
    void delete_zeros() const {
        using std::abs;
        std::vector<std::pair<unsigned, unsigned>> to_delete;
        for (auto& [row_num, row] : map_) {
            for (auto& [col_num, elem] : row) {
                if (abs(elem) < eps_) {
                    to_delete.push_back({row_num, col_num});
                }
            }
        }
        for (const auto& [row_num, col_num] : to_delete) {
            map_[row_num].erase(col_num);
            if (map_[row_num].empty()) {
                map_.erase(row_num);
            }
        }
    }

    /// Оператор равенства
    friend bool operator== (const Matrix& lhs, const Matrix& rhs) {
        lhs.delete_zeros();
        long long l_size = 0;
        for (const auto& [_, row] : lhs.map_) {
            for (const auto& [_, elem] : row) {
                l_size += 1;
            }
        }
        rhs.delete_zeros();
        long long r_size = 0;
        for (const auto& [_, row] : rhs.map_) {
            for (const auto& [_, elem] : row) {
                r_size += 1;
            }
        }
        if (l_size != r_size) {
            return false;
        }
        for (auto& [row_num, row] : lhs.map_) {
            for (auto& [col_num, elem] : row) {
                if (rhs(row_num, col_num) != elem) {
                    return false;
                }
            }
        }
        return true;
    }

    /// Оператор неравенства
    friend bool operator!= (const Matrix& lhs, const Matrix& rhs) {
        return !(lhs == rhs);
    }

protected:
    /// Контейнер элементов
    mutable M<unsigned, M<unsigned, T>> map_;

    /// Минимальный модуль элемента
    double eps_;

    /// Количество строк
    unsigned rows_num_;

    /// Количество столбцов
    unsigned cols_num_;

    /// Сделанные срезы
    mutable std::set<const Matrix_proxy<T, M>*> proxy_;
};

/**
    \brief Класс с тестами для класса Matrix

    Данный класс содержит тесты для класса Matrix.
*/
class MatrixTest {
public:
    void operator() () {
        if (Matrix<int>::make_unary(2, 3, 0.5) != Matrix<int>({{1, {{1, 1}}}, {2, {{2, 1}}}}, 
            2, 3, 0.5))
        {
            throw test_failed_error("unary test failed");
        }
        if (Matrix<int>::make_ones(2, 1, 0.5) != Matrix<int>({{1, {{1, 1}}}, {2, {{1, 1}}}}, 
            2, 1, 0.5))
        {
            throw test_failed_error("ones test failed");
        }
        if (Matrix<int>::make_zeros(5, 6, 0.5) != Matrix<int>({}, 
            5, 6, 0.5))
        {
            throw test_failed_error("zeros test failed");
        }
        if (Matrix<RationalNumber<int>>::from_file("matrix_sample_rat", 0.5) != Matrix<RationalNumber<int>>(
            {
                {1, {{1, RationalNumber(100)}}},
                {6000, {{2, RationalNumber(23, 5)}}},
                {7, {{1, RationalNumber(5, 3)}}},
                {22, {{2, RationalNumber(44)}}}
            }, 
            50000, 6000, 0.5))
        {
            throw test_failed_error("from_file test failed");
        }
        if (Matrix<int>::make_ones(2, 1, 0.5).to_file_string() != 
            "matrix integer 2 1\n1 1 1\n2 1 1\n")
        {
            throw test_failed_error("to_file_string test failed");
        }
        if (~Matrix<int>({{1, {{2, 4}}}}, 3, 4, 0.5) != 
            Matrix<int>({{2, {{1, 4}}}}, 4, 3, 0.5)) 
        {
            throw test_failed_error("transpose test failed");
        }
        if (-Matrix<int>({{1, {{2, 4}}}}, 3, 4, 0.5) != 
            Matrix<int>({{1, {{2, -4}}}}, 3, 4, 0.5)) 
        {
            throw test_failed_error("minus test failed");
        }
        if (Matrix<int>({{1, {{1, 3}, {2, 4}}}}, 3, 4, 0.5) +
            Matrix<int>({{1, {{2, 3}, {3, 4}}}}, 3, 4, 0.5) !=
            Matrix<int>({{1, {{1, 3}, {2, 7}, {3, 4}}}}, 3, 4, 0.5))
        {
            throw test_failed_error("add test failed");
        }
        if (Matrix<int>({{1, {{1, 3}, {2, 4}}}}, 3, 4, 0.5) -
            Matrix<int>({{1, {{2, 3}, {3, 4}}}}, 3, 4, 0.5) !=
            Matrix<int>({{1, {{1, 3}, {2, 1}, {3, -4}}}}, 3, 4, 0.5))
        {
            throw test_failed_error("sub test failed");
        }
        if (Matrix<int>({{1, {{1, 1}, {2, 2}}}, {2, {{1, 3}, {2, 4}}}}, 2, 2, 0.5) *
            Matrix<int>({{1, {{1, 5}}}, {2, {{1, 6}}}}, 2, 1, 0.5) !=
            Matrix<int>({{1, {{1, 17}}}, {2, {{1, 39}}}}, 2, 2, 0.5))
        {
            throw test_failed_error("mul test failed");
        }
        if (Matrix<int>({{1, {{1, 1}, {2, 2}}}, {2, {{1, 3}, {2, 4}}}}, 2, 2, 0.5) * 3 !=
            Matrix<int>({{1, {{1, 3}, {2, 6}}}, {2, {{1, 9}, {2, 12}}}}, 2, 2, 0.5))
        {
            throw test_failed_error("mul with num test failed");
        }
        auto test = Matrix<int>({{1, {{1, 1}, {2, 2}}}, {2, {{1, 3}, {2, 4}}}}, 2, 2, 0.5);
        test[std::make_pair(1, 2)] = 8;
        if (test(1, 2) != 8) {
            throw test_failed_error("change elem test failed");
        }
        auto test1 = Matrix<double>({{1, {{1, 1}, {2, 2}}}, {2, {{1, 3}, {2, 4}}}}, 2, 2, 0.5);
        test1[std::make_pair(1, 2)] = 0.25;
        if (test1(1, 2) != 0) {
            throw test_failed_error("eps test failed");
        }
        std::cout << "matrix tests completed" << std::endl;
    }
};

/// Класс среза
template <class T, template <class...> class M>
class Matrix_proxy {
public:
    /// Конструктор
    Matrix_proxy(Matrix<T, M>* parent, unsigned start_row, unsigned end_row,
        unsigned start_col, unsigned end_col) : parent_(parent),
        start_row_(start_row), end_row_(end_row), start_col_(start_col), end_col_(end_col) {}

    /// Деструктор
    ~Matrix_proxy() {
        parent_->detach_proxy(this);
    }

    /// Метод удаления родителя
    void parent_deleted() const {
        parent_ = nullptr;
    }

    /// Метод константного доступа к элементу
    T operator() (unsigned row_num, unsigned col_num) const {
        if (parent_ == nullptr) {
            throw parent_deleted_error("parent deleted");
        }
        if (row_num < 1 || row_num > (end_row_ - start_row_ + 1)) {
            throw invalid_proxy_index_error("invalid row index", this, {row_num, col_num});
        }
        if (col_num < 1 || col_num > (end_col_ - start_col_ + 1)) {
            throw invalid_proxy_index_error("invalid col index", this, {row_num, col_num});
        }
        return (*parent_)(row_num - 1 + start_row_, col_num - 1 + start_col_);
    }

    /// Метод получения родителя
    const Matrix<T, M>* get_parent() const {
        return parent_;
    }

    /// Метод получения границ строк
    std::pair<unsigned, unsigned> get_row_index() const {
        return {start_row_, end_row_};
    }

    /// Метод получения границ столбцов
    std::pair<unsigned, unsigned> get_col_index() const {
        return {start_col_, end_col_};
    }

protected:
    /// родитель
    mutable Matrix<T, M>* parent_;
    /// Границы среза
    unsigned start_row_, end_row_, start_col_, end_col_;
};

/**
    \brief Класс с тестами для срезов

    Данный класс содержит тесты для срезов.
*/
class ProxyTest {
public:
    void operator() () {
        auto test = Matrix<int>(
            {
                {1, {{1, 1}, {2, 2}, {3, 3}}}, 
                {2, {{1, 4}, {2, 5}, {3, 6}}},
                {3, {{1, 7}, {2, 8}, {3, 9}}}
            }, 
            100, 100, 0.5);
        auto proxy = *test[{-1, 2, 2, 4}];
        if (proxy(1, 1) != 2 || proxy(1, 2) != 3 || proxy(2, 1) != 5 || proxy(2, 2) != 6) {
            throw test_failed_error("proxy elem test failed");
        }
        auto pr_matr = Matrix<int>(proxy, 0.5);
        if (pr_matr != Matrix<int>({{1, {{1, 2}, {2, 3}}}, {2, {{1, 5}, {2, 6}}}}, 2, 3, 0.5)) {
            throw test_failed_error("proxy to matrix test failed");
        }
        if (Matrix<int>(*(test[Matrix_row_coord(2)]), 0.5) != 
            Matrix<int>({{1, {{1, 4}, {2, 5}, {3, 6}}}}, 1, 100, 0.5))
        {
            throw test_failed_error("matrix row test failed");
        }
        if (Matrix<int>(*(test[Matrix_col_coord(2)]), 0.5) != 
            Matrix<int>({{1, {{1, 2}}}, {2, {{1, 5}}}, {3, {{1, 8}}}}, 100, 1, 0.5))
        {
            throw test_failed_error("matrix row test failed");
        }
        {
            auto test1 = new Matrix<int>(3, 4, 0.5);
            auto pr1 = (*test1)[Matrix_row_coord(2)];
            delete test1;
            bool caught = false;
            try {
                (*pr1)(1, 2);
            } catch (parent_deleted_error& ex) {
                caught = true;
            }
            if (!caught) {
                throw test_failed_error("parent deleted test failed");
            }
        }
        {
            auto test1 = new Matrix<int>(3, 4, 0.5);
            auto pr1 = (*test1)[Matrix_row_coord(2)];
            delete pr1;
            delete test1;
        }
        std::cout << "proxy tests completed" << std::endl;
    }
};