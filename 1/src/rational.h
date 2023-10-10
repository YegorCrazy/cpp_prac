#include <bitset>
#include <exception>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <cmath>

#include <iostream>

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

/**
    \brief Исключение переполнения

    Данный класс является исключением для ситуации переполнения
    целочисленного типа при проведении операции с рациональным число.
*/
class overflow_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
    \brief Исключение недопустимой строки

    Данный класс является исключением для ситуации, когда для создания
    рационального числа используется недопустимая строка (например, не
    содержащая символ '/').
*/
class invalid_string_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
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
class RationalNumber;

template <class T>
RationalNumber<T> make_canonical(RationalNumber<T> arg);

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
    RationalNumber(const char* numerator, const char* denominator) : 
        RationalNumber(std::stoll(numerator), std::stoll(denominator))
    {}

    /// Конструктор из строчного представления рационального числа
    RationalNumber(const char* init_c) {
        std::string init = std::string(init_c);
        int pos = init.find("/");
        if (pos == std::string::npos) {
            throw invalid_string_error("no slash in string");
        }
        auto new_one = RationalNumber(init.substr(0, pos).c_str(), init.substr(pos + 1, init.size() - pos - 1).c_str());
        numerator_ = new_one.get_numerator();
        denominator_ = new_one.get_denominator();
    }

    /// Конструктор присваивания
    RationalNumber& operator= (RationalNumber& other) {
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
    RationalNumber operator- () {
        T new_numerator;
        if (__builtin_mul_overflow(numerator_, -1, &new_numerator)) {
            throw overflow_error("type overflow");
        }
        numerator_ = new_numerator;
        return *this;
    }

    /// Оператор +=
    RationalNumber operator+= (const RationalNumber& rhs) {
        auto eq_denom = make_equal_denominator(*this, rhs);
        if (__builtin_add_overflow(eq_denom.first.numerator_, eq_denom.second.numerator_, &numerator_)) {
            throw overflow_error("type overflow");
        }
        denominator_ = eq_denom.first.denominator_;
        make_canonical();
        return *this;
    }

    /// Оператор -=
    RationalNumber operator-= (const RationalNumber& rhs) {
        auto eq_denom = make_equal_denominator(*this, rhs);
        if (__builtin_sub_overflow(eq_denom.first.numerator_, eq_denom.second.numerator_, &numerator_)) {
            throw overflow_error("type overflow");
        }
        denominator_ = eq_denom.first.denominator_;
        make_canonical();
        return *this;
    }

    /// Оператор *=
    RationalNumber operator*= (const RationalNumber& rhs) {
        T new_denominator;
        if (__builtin_mul_overflow(denominator_, rhs.denominator_, &new_denominator)) {
            throw overflow_error("type overflow");
        }
        if (__builtin_mul_overflow(numerator_, rhs.numerator_, &numerator_)) {
            throw overflow_error("type overflow");
        }
        denominator_ = new_denominator;
        make_canonical();
        return *this;
    }

    /// Оператор /=
    RationalNumber operator/= (const RationalNumber& rhs) {
        T new_denominator;
        if (__builtin_mul_overflow(denominator_, rhs.numerator_, &new_denominator)) {
            throw overflow_error("type overflow");
        }
        if (__builtin_mul_overflow(numerator_, rhs.denominator_, &numerator_)) {
            throw overflow_error("type overflow");
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
            throw overflow_error("type overflow");
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
        throw overflow_error("type overflow");
    }
    T new_numerator_1;
    if (__builtin_mul_overflow(first_mul, arg1.get_numerator(), &new_numerator_1)) {
        throw overflow_error("type overflow");
    }
    T2 new_numerator_2;
    if (__builtin_mul_overflow(second_mul, arg2.get_numerator(), &new_numerator_2)) {
        throw overflow_error("type overflow");
    }
    return {RationalNumber<T>{new_numerator_1, new_denominator}, RationalNumber<T2>{new_numerator_2, new_denominator}};
}

/**
    \brief Класс с тестами для класса RationalNumber

    Данный класс содержит тесты для класса RationalNumber.
*/
class RationalNumberTest {
public:
    void operator() () {
        auto x = RationalNumber<int>("1", "2");
        if (x != RationalNumber(1, 2)) {
            throw test_failed_error("constructing from 2 strings failed");
        }
        if (RationalNumber<int>("1/2") != RationalNumber(1, 2)) {
            throw test_failed_error("constructing from 1 string failed");
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
        std::cout << "tests finished" << std::endl;
    }
};