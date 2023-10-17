#pragma once

#include <bitset>
#include <exception>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <cmath>

#include <cstdlib>

#include <iostream>

template <class T>
class RationalNumber;

/**
    \brief Исключение деления на 0

    Данный класс является исключением для ситуации деления на 0.
*/
class zero_division_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
    \brief Исключение отрицательного знаменателя

    Данный класс является исключением для ситуации, когда у 
    рационального числа обнаруживается отрицательный знаменатель.
*/
class negative_denominator_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

enum class operation {
    add,
    sub,
    mul,
    div,
    eq_denom,
    to_int,
};

/**
    \brief Исключение переполнения

    Данный класс является исключением для ситуации переполнения
    целочисленного типа при проведении операции с рациональным числом.
*/
template <class T1, class T2>
class overflow_error : public std::runtime_error {
public:
    overflow_error(const char* what, RationalNumber<T1> n1, RationalNumber<T2> n2, operation oper) :
        std::runtime_error{what}, n1{n1}, n2{n2}, oper{oper} {}
    RationalNumber<T1> n1;
    RationalNumber<T2> n2;
    operation oper;
};

/**
    \brief Исключение недопустимой строки

    Данный класс является исключением для ситуации, когда для создания
    рационального числа используется недопустимая строка (например, не
    содержащая символ '/').
*/
class invalid_string_error : public std::runtime_error {
public:
    invalid_string_error(std::string what, std::string reason) :
        std::runtime_error{what}, reason{reason} {}
    std::string reason;
};

/**
    \brief Исключение ошибки теста

    Данный класс является исключением для провала теста.
*/
class test_failed_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
    \brief Вычисление НОД

    Данная функция вычисляет НОД двух чисел.
*/
template <class T>
T get_max_delim(T a, T b) {
    if (a == b) {
        return a;
    }
    T tmp;
    while (b != 0) {
        tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

template <class T>
T from_string(std::string s) {
    T res = 0;
    int index = 0;
    bool neg = false;
    while (index < s.size()) {
        if (index == 0 && s[0] == '-') {
            neg = true;
            continue;
        }
        if (s[index] < '0' || s[index] > '9') {
            throw std::runtime_error("invalid string characters");
        }
        int current_digit = s[index] - '0';
        if (__builtin_mul_overflow(res, 10, &res)) {
            throw std::runtime_error("number too big");
        }
        if (__builtin_add_overflow(res, current_digit, &res)) {
            throw std::runtime_error("number too big");
        }
        index += 1;
    }
    if (neg) {
        if (__builtin_mul_overflow(res, -1, &res)) {
            throw std::runtime_error("number too big");
        }
    }
    return res;
}

template <class T>
RationalNumber<T> make_canonical(RationalNumber<T> arg);

template <class T>
std::string to_string(RationalNumber<T>& num) {
    return std::string(num);
}

/**
    \brief Рациональное число

    Данный класс представляет рациональное число.
*/
template <class T = int>
class RationalNumber {
protected:
    /// Числитель числа
    T numerator_;

    /// Знаменатель числа
    T denominator_;

public:
    /// Конструктор по умолчанию
    RationalNumber() :
        numerator_{0}, denominator_{1} 
    {
        static_assert(std::is_integral<T>::value, "invalid type (must be integral)");
    }

    /// Конструктор из целого числа
    RationalNumber(T numerator) :
        numerator_{numerator}, denominator_{1}
    {
        static_assert(std::is_integral<T>::value, "invalid type (must be integral)");
    }

    /// Конструктор по числителю и знаменателю
    RationalNumber(T numerator, T denominator) :
        numerator_{numerator}, denominator_{denominator} 
    {
        static_assert(std::is_integral<T>::value, "invalid type (must be integral)");
        if (denominator == 0) {
            throw zero_division_error("denominator cannot be zero");
        }
        if (denominator < 0) {
            throw negative_denominator_error("denominator cannot be negative");
        }

    }
    
    /// Конструктор копирования
    RationalNumber(const RationalNumber& other) :
        numerator_{other.numerator_}, denominator_{other.denominator_} {}

    /// Конструктор копирования из рационального числа другого типа
    template<class T2>
    RationalNumber(const RationalNumber<T2>& other) {
        static_assert(sizeof(T2) <= sizeof(T), "type size decreases");
        static_assert(!(std::is_signed_v<T> ^ std::is_signed_v<T2>), "type signess differs");
        numerator_ = other.get_numerator();
        denominator_ = other.get_denominator();
    }
    
    /// Конструктор копирования по rvalue-ссылке
    RationalNumber(RationalNumber&& other) :
        numerator_{other.numerator_}, denominator_{other.denominator_} {}

    /// Конструктор из строчного представления числителя и знаменателя
    RationalNumber(const char* numerator, const char* denominator) {
        try {
            numerator_ = from_string<T>(numerator);
        } catch (std::runtime_error& ex) {
            throw invalid_string_error("invalid string", numerator);
        }
        try {
            denominator_ = from_string<T>(denominator);
        } catch (std::runtime_error& ex) {
            throw invalid_string_error("invalid string", denominator);
        }
    }

    /// Конструктор из строчного представления рационального числа
    RationalNumber(const char* init_c) {
        std::string init = std::string(init_c);
        RationalNumber new_one;
        int pos = init.find("/");
        if (pos == std::string::npos) {
            try {
                T numerator = from_string<T>(init);
                new_one = RationalNumber(numerator);
            } catch (std::runtime_error& ex) {
                throw invalid_string_error("invalid string", init);
            }
        } else {
            new_one = RationalNumber(init.substr(0, pos).c_str(), init.substr(pos + 1, init.size() - pos - 1).c_str());
        }
        numerator_ = new_one.get_numerator();
        denominator_ = new_one.get_denominator();
    }

    /// Конструктор присваивания
    RationalNumber& operator= (RationalNumber& other) {
        numerator_ = other.numerator_;
        denominator_ = other.denominator_;
        return *this;
    }

    /// Конструктор присваивания по rvalue-ссылке
    RationalNumber& operator= (RationalNumber&& other) {
        numerator_ = other.numerator_;
        denominator_ = other.denominator_;
        return *this;
    }

    /// Метод получения числителя
    T get_numerator() const {
        return numerator_;
    }

    /// Метод получения знаменателя
    T get_denominator() const {
        return denominator_;
    }

    /// Оператор преобразования в строку
    operator std::string() const {
        return std::to_string(numerator_) + "/" + std::to_string(denominator_);
    }

    /// Оператор приведения к каноническому виду
    void make_canonical() {
        T max_delim = get_max_delim(numerator_, denominator_);
        numerator_ = numerator_ / max_delim;
        denominator_ = denominator_ / max_delim;
    }

    /// Оператор унарного минуса
    RationalNumber operator- () const {
        T new_numerator;
        if (__builtin_mul_overflow(numerator_, -1, &new_numerator)) {
            throw overflow_error("type overflow", *this, RationalNumber(-1), operation::mul);
        }
        return RationalNumber(new_numerator, denominator_);
    }

    /// Оператор +=
    RationalNumber operator+= (const RationalNumber& rhs) {
        auto eq_denom = make_equal_denominator(*this, rhs);
        if (__builtin_add_overflow(eq_denom.first.numerator_, eq_denom.second.numerator_, &numerator_)) {
            throw overflow_error("type overflow", *this, rhs, operation::add);
        }
        denominator_ = eq_denom.first.denominator_;
        make_canonical();
        return *this;
    }

    /// Оператор -=
    RationalNumber operator-= (const RationalNumber& rhs) {
        auto eq_denom = make_equal_denominator(*this, rhs);
        if (__builtin_sub_overflow(eq_denom.first.numerator_, eq_denom.second.numerator_, &numerator_)) {
            throw overflow_error("type overflow", *this, rhs, operation::sub);
        }
        denominator_ = eq_denom.first.denominator_;
        make_canonical();
        return *this;
    }

    /// Оператор *=
    RationalNumber operator*= (const RationalNumber& rhs) {
        T new_denominator;
        if (__builtin_mul_overflow(denominator_, rhs.denominator_, &new_denominator)) {
            throw overflow_error("type overflow", *this, rhs, operation::mul);
        }
        if (__builtin_mul_overflow(numerator_, rhs.numerator_, &numerator_)) {
            throw overflow_error("type overflow", *this, rhs, operation::mul);
        }
        denominator_ = new_denominator;
        make_canonical();
        return *this;
    }

    /// Оператор /=
    RationalNumber operator/= (const RationalNumber& rhs) {
        T new_denominator;
        if (__builtin_mul_overflow(denominator_, rhs.numerator_, &new_denominator)) {
            throw overflow_error("type overflow", *this, rhs, operation::div);
        }
        if (__builtin_mul_overflow(numerator_, rhs.denominator_, &numerator_)) {
            throw overflow_error("type overflow", *this, rhs, operation::div);
        }
        denominator_ = new_denominator;
        make_canonical();
        return *this;
    }

    /// Оператор префиксного инкремента
    RationalNumber& operator++ () {
        operator+=(1);
        return *this;
    }
 
    /// Оператор постфиксного инкремента
    RationalNumber operator++ (int) {
        RationalNumber old = *this;
        operator++();
        return old;
    }
 
    /// Оператор префиксного декремента
    RationalNumber& operator-- () {
        operator-=(1);
        return *this;
    }
 
    /// Оператор постфиксного декремента
    RationalNumber operator-- (int) {
        RationalNumber old = *this;
        operator--();
        return old;
    }

    /// Оператор сложения
    template <class T2>
    friend auto operator+ (RationalNumber lhs, const RationalNumber<T2>& rhs) {
        RationalNumber rhs_new = RationalNumber(rhs);
        lhs += rhs_new;
        return lhs;
    }

    /// Оператор вычитания
    template <class T2>
    friend auto operator- (RationalNumber lhs, const RationalNumber<T2>& rhs) {
        RationalNumber rhs_new = RationalNumber(rhs);
        lhs -= rhs_new;
        return lhs;
    }

    /// Оператор умножения
    template <class T2>
    friend auto operator* (RationalNumber lhs, const RationalNumber<T2>& rhs) {
        RationalNumber rhs_new = RationalNumber(rhs);
        lhs *= rhs_new;
        return lhs;
    }

    /// Оператор деления
    template <class T2>
    friend auto operator/ (RationalNumber lhs, const RationalNumber<T2>& rhs) {
        RationalNumber rhs_new = RationalNumber(rhs);
        lhs /= rhs_new;
        return lhs;
    }

    /// Оператор равенства
    template <class T2>
    friend bool operator== (const RationalNumber& lhs, const RationalNumber<T2>& rhs) {
        RationalNumber lhs_can = ::make_canonical(lhs);
        RationalNumber rhs_can = ::make_canonical(rhs);
        return (lhs_can.get_numerator() == rhs_can.get_numerator() && lhs_can.get_denominator() == rhs_can.get_denominator());
    }

    /// Оператор неравенства
    template <class T2>
    friend bool operator!= (const RationalNumber& lhs, const RationalNumber<T2>& rhs) {
        return !(lhs == rhs);
    }

    /// Оператор "меньше"
    friend bool operator< (const RationalNumber& lhs, const RationalNumber& rhs) {
        auto eq_denom = make_equal_denominator(lhs, rhs);
        return eq_denom.first.get_numerator() < eq_denom.second.get_numerator();
    }

    /// Оператор "меньше" для чисел разных типов
    template <class T2>
    friend bool operator< (const RationalNumber& lhs, const RationalNumber<T2>& rhs) {
        if (lhs == rhs) {
            return false;
        }
        if (lhs.get_numerator() > 0 && rhs.get_numerator() < 0) {
            return false;
        }
        if (lhs.get_numerator() < 0 && rhs.get_numerator() > 0) {
            return true;
        }
        auto eq_denom = make_equal_denominator(lhs, rhs);
        return eq_denom.first.get_numerator() < eq_denom.second.get_numerator();
    }

    /// Оператор "больше"
    template <class T2>
    friend bool operator> (const RationalNumber& lhs, const RationalNumber<T2>& rhs) {
        return rhs < lhs;
    }

    /// Оператор "меньше или равно"
    template <class T2>
    friend bool operator<= (const RationalNumber& lhs, const RationalNumber<T2>& rhs) {
        return !(rhs < lhs);
    }

    /// Оператор "больше или равно"
    template <class T2>
    friend bool operator>= (const RationalNumber& lhs, const RationalNumber<T2>& rhs) {
        return !(lhs < rhs);
    }

    /// Оператор вывода на поток вывода
    friend std::ostream& operator<<(std::ostream &os, const RationalNumber& arg) {
        os << std::string(arg);
        return os;
    }

    /// Оператор преобразования к int
    explicit operator int() const {
        long long res = numerator_ / denominator_;
        if (res > std::numeric_limits<int>::max() || res < std::numeric_limits<int>::min()) {
            throw overflow_error("type overflow", *this, RationalNumber(0), operation::to_int);
        }
        return res;
    }

    /// Оператор преобразования к double
    explicit operator double() const {
        return (double)numerator_ / denominator_;
    }

    /// Оператор округления вниз
    T floor() const {
        return std::floor(double(*this));
    }

    /// Оператор округления
    T round() const {
        return std::round(double(*this));
    }
};

/**
    \brief Приведение к каноническому виду

    Данная функция приводит рациональное число к каноническому виду и
    возвращает его копию.
*/
template <class T>
RationalNumber<T> make_canonical(RationalNumber<T> arg) {
    arg.make_canonical();
    return arg;
}

/**
    \brief Приведение к общему знаменателю

    Данная функция приводит два рациональных числа к общему знаменателю и
    возвращает их копии.
*/
template <class T, class T2>
std::pair<RationalNumber<T>, RationalNumber<T2>> make_equal_denominator(const RationalNumber<T>& arg1, const RationalNumber<T2>& arg2) {
    auto max_delim = get_max_delim(arg1.get_denominator(), arg2.get_denominator());
    auto first_mul = arg2.get_denominator() / max_delim;
    auto second_mul = arg1.get_denominator() / max_delim;
    T new_denominator;
    if (__builtin_mul_overflow(first_mul, arg1.get_denominator(), &new_denominator)) {
        throw overflow_error("type overflow", arg1, arg2, operation::eq_denom);
    }
    T new_numerator_1;
    if (__builtin_mul_overflow(first_mul, arg1.get_numerator(), &new_numerator_1)) {
        throw overflow_error("type overflow", arg1, arg2, operation::eq_denom);
    }
    T2 new_numerator_2;
    if (__builtin_mul_overflow(second_mul, arg2.get_numerator(), &new_numerator_2)) {
        throw overflow_error("type overflow", arg1, arg2, operation::eq_denom);
    }
    return {RationalNumber<T>{new_numerator_1, new_denominator}, RationalNumber<T2>{new_numerator_2, new_denominator}};
}

template <class T>
RationalNumber<T> abs(const RationalNumber<T>& num) {
    if (num.get_numerator() < 0) {
        return -num;
    }
    return num;
}

/**
    \brief Класс с тестами для класса RationalNumber

    Данный класс содержит тесты для класса RationalNumber.
*/
class RationalNumberTest {
public:
    void operator() () {
        auto x = RationalNumber<int>("100", "2");
        if (x != RationalNumber(100, 2)) {
            throw test_failed_error("constructing from 2 strings failed");
        }
        if (RationalNumber<int>("1/2") != RationalNumber(1, 2)) {
            throw test_failed_error("constructing from 1 string failed");
        }
        bool catched = false;
        try {
            auto cc = RationalNumber<long long>("100000000000000000000000000000000000000000000000000000000000000000");
        } catch (invalid_string_error& ex) {
            catched = true;
        }
        if (!catched) {
            throw test_failed_error("big string test failed");
        }
        RationalNumber a(3, 5);
        RationalNumber b(5, 6);
        if (a + b != RationalNumber(43, 30)) {
            throw test_failed_error("adding test failed");
        }
        if (a - b != RationalNumber(-7, 30)) {
            throw test_failed_error("substracting test failed");
        }
        if (a * b != RationalNumber(1, 2)) {
            throw test_failed_error("multiplying test failed");
        }
        if (a / b != RationalNumber(18, 25)) {
            throw test_failed_error("dividing test failed");
        }
        if ((RationalNumber(3, 5) += b) != RationalNumber(43, 30)) {
            throw test_failed_error("add-copy test failed");
        }
        if ((RationalNumber(3, 5) -= b) != RationalNumber(-7, 30)) {
            throw test_failed_error("sub-copy test failed");
        }
        if ((RationalNumber(3, 5) *= b) != RationalNumber(1, 2)) {
            throw test_failed_error("mul-copy test failed");
        }
        if ((RationalNumber(3, 5) /= b) != RationalNumber(18, 25)) {
            throw test_failed_error("div-copy test failed");
        }
        try {
            auto cc = RationalNumber(1000000000, 1) * RationalNumber(1000000000, 1);
        } catch (overflow_error<int, int>& ex) {
            //std::cout << ex.n1 << " " << ex.n2 << std::endl;
            catched = true;
        }
        if (!catched) {
            throw test_failed_error("overflow test failed");
        }
        if (!(a < b)) {
            throw test_failed_error("less test failed");
        }
        if (!(a <= b)) {
            throw test_failed_error("less or equal test failed");
        }
        if (a > b) {
            throw test_failed_error("more test failed");
        }
        if (a >= b) {
            throw test_failed_error("more or equal test failed");
        }
        if (a == b) {
            throw test_failed_error("equal test failed");
        }
        if (!(a != b)) {
            throw test_failed_error("not equal test failed");
        }
        if (b.floor() != 0) {
            throw test_failed_error("floor test failed");
        }
        if (b.round() != 1) {
            throw test_failed_error("round test failed");
        }
        if (std::abs(double(b) - ((double)5 / 6)) > 0.00001) {
            throw test_failed_error("double test failed");
        }
        if (int(b) != 0) {
            throw test_failed_error("int test failed");
        }
        if (std::string(b) != "5/6") {
            throw test_failed_error("string test failed");
        }
        std::cout << "rational tests completed" << std::endl;
    }
};
