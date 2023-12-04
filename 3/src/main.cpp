#include "functions.h"
#include <iostream>

int main() {
    auto pol = TFunction::construct(FunctionType::polynomial, {1, 2, 3});
    auto constant = TFunction::construct(FunctionType::constant, -10);
    auto x = pol + constant;
    std::cout << x->toString() << std::endl;
    std::cout << x->value(0) << std::endl;
    std::cout << x->value(1) << std::endl;
    std::cout << x->value(2) << std::endl;

    std::cout << "x root is " << gradientDescent(x, 1000) << std::endl;

    return 0;
}