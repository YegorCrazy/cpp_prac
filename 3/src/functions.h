#include <memory>
#include <stdexcept>
#include <vector>
#include <cmath>

enum class FunctionType {
    identical,
    constant,
    power,
    exponential,
    polynomial
};

enum class Operation {
    add,
    sub,
    mul, 
    div
};

class TFunction {
public:
    virtual double value(double arg) const = 0;
    virtual double derivative(double arg) const = 0;
    virtual std::string toString() const = 0;
    static std::shared_ptr<TFunction> 
        construct(FunctionType function_type);
    static std::shared_ptr<TFunction> 
        construct(FunctionType function_type, double arg);
    static std::shared_ptr<TFunction> 
        construct(FunctionType function_type, std::vector<double> args);

    friend std::shared_ptr<TFunction> operator+(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b);
    friend std::shared_ptr<TFunction> operator-(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b);
    friend std::shared_ptr<TFunction> operator*(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b);
    friend std::shared_ptr<TFunction> operator/(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b);
};

class IdenticalFunction : public TFunction {
public:
    IdenticalFunction() {}
    double value(double arg) const override {
        return arg;
    }
    double derivative(double arg) const override {
        return 1;
    }
    std::string toString() const override {
        return "x";
    }
};

class ConstantFunction : public TFunction {
public:
    ConstantFunction(double constant) : constant_(constant) {};
    double value(double arg) const override {
        return constant_;
    }
    double derivative(double arg) const override {
        return 0;
    }
    std::string toString() const override {
        return std::to_string(constant_);
    }
private:
    double constant_;
};

class PowerFunction : public TFunction {
public:
    PowerFunction(double power) : power_(power) {};
    double value(double arg) const override {
        return pow(arg, power_);
    }
    double derivative(double arg) const override {
        return power_ * pow(arg, power_ - 1);
    }
    std::string toString() const override {
        return "x^" + std::to_string(power_);
    }
private:
    double power_;
};

class ExponentialFunction : public TFunction {
public:
    ExponentialFunction(double constant) : constant_(constant) {};
    double value(double arg) const override {
        return pow(constant_, arg);
    }
    double derivative(double arg) const override {
        return pow(constant_, arg) * log(constant_);
    }
    std::string toString() const override {
        return std::to_string(constant_) + "^x";
    }
private:
    double constant_;
};

class PolynomialFunction : public TFunction {
public:
    PolynomialFunction(const std::vector<double>& coef) : coef_(coef) {};
    double value(double arg) const override {
        double res = 0;
        for (int i = 0; i < coef_.size(); ++i) {
            res += pow(arg, i) * coef_[i];
        }
        return res;
    }
    double derivative(double arg) const override {
        double res = 0;
        for (int i = 1; i < coef_.size(); ++i) {
            res += i * pow(arg, i - 1) * coef_[i];
        }
        return res;
    }
    std::string toString() const override {
        std::string res = "";
        for (int i = 0; i < coef_.size(); ++i) {
            if (i > 0) {
                res += " + ";
            }
            res += std::to_string(coef_[i]) + "x^" + std::to_string(i);
        }
        return res;
    }
private:
    std::vector<double> coef_;
};

class OperationOnFunctions : public TFunction {
public:
    OperationOnFunctions(std::shared_ptr<TFunction> first,
        std::shared_ptr<TFunction> second, Operation oper) :
        first_(first), second_(second), oper_(oper) {}
    double value(double arg) const override;
    double derivative(double arg) const override;
    std::string toString() const override;
private:
    std::shared_ptr<TFunction> first_;
    std::shared_ptr<TFunction> second_;
    Operation oper_;
};