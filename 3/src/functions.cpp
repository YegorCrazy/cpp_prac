#include "functions.h"
#include <memory>
#include <stdexcept>

std::shared_ptr<TFunction> TFunction::construct(FunctionType function_type) {
    if (function_type == FunctionType::identical) {
        return std::make_shared<IdenticalFunction>();
    } else {
        throw std::logic_error("wrong function type");
    }
}

std::shared_ptr<TFunction> TFunction::construct(FunctionType function_type, double arg) {
    if (function_type == FunctionType::constant) {
        return std::make_shared<ConstantFunction>(arg);
    } else if (function_type == FunctionType::power) {
        return std::make_shared<PowerFunction>(arg);
    } else if (function_type == FunctionType::exponential) {
        return std::make_shared<ExponentialFunction>(arg);
    } else {
        throw std::logic_error("wrong function type");
    }
}

std::shared_ptr<TFunction> TFunction::construct(FunctionType function_type, std::vector<double> args) {
    if (function_type == FunctionType::polynomial) {
        return std::make_shared<PolynomialFunction>(args);
    } else {
        throw std::logic_error("wrong function type");
    }
}

std::shared_ptr<TFunction> operator+(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b) {
    return std::make_shared<OperationOnFunctions>(a, b, Operation::add);
}
std::shared_ptr<TFunction> operator-(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b) {
    return std::make_shared<OperationOnFunctions>(a, b, Operation::sub);
}
std::shared_ptr<TFunction> operator*(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b) {
    return std::make_shared<OperationOnFunctions>(a, b, Operation::mul);
}
std::shared_ptr<TFunction> operator/(std::shared_ptr<TFunction> a, std::shared_ptr<TFunction> b) {
    return std::make_shared<OperationOnFunctions>(a, b, Operation::div);
}

double OperationOnFunctions::value(double arg) const {
    if (oper_ == Operation::add) {
        return first_->value(arg) + second_->value(arg);
    } else if (oper_ == Operation::sub) {
        return first_->value(arg) - second_->value(arg);
    } else if (oper_ == Operation::mul) {
        return first_->value(arg) * second_->value(arg);
    } else if (oper_ == Operation::div) {
        return first_->value(arg) / second_->value(arg);
    } else {
        throw std::logic_error("unknown operation");
    }
}

double OperationOnFunctions::derivative(double arg) const {
    if (oper_ == Operation::add) {
        return first_->derivative(arg) + second_->derivative(arg);
    } else if (oper_ == Operation::sub) {
        return first_->derivative(arg) - second_->derivative(arg);
    } else if (oper_ == Operation::mul) {
        return second_->value(arg) * first_->derivative(arg) + first_->value(arg) * second_->derivative(arg);
    } else if (oper_ == Operation::div) {
        return (second_->value(arg) * first_->derivative(arg) - first_->value(arg) * second_->derivative(arg))
            / pow(second_->value(arg), 2);
    } else {
        throw std::logic_error("unknown operation");
    }
}

std::string OperationOnFunctions::toString() const {
    if (oper_ == Operation::add) {
        return first_->toString() + " + " + second_->toString();
    } else if (oper_ == Operation::sub) {
        return first_->toString() + " - " + second_->toString();
    } else if (oper_ == Operation::mul) {
        return first_->toString() + " * " + second_->toString();
    } else if (oper_ == Operation::div) {
        return first_->toString() + " / " + second_->toString();
    } else {
        throw std::logic_error("unknown operation");
    }
}