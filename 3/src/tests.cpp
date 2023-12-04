#include <gtest/gtest.h>
#include "functions.h"

using namespace std;

TEST(ConstructAndStringTests, IdenticalTest) {
    ASSERT_TRUE(TFunction::construct(FunctionType::identical)->toString() == "x");
}

TEST(ConstructAndStringTests, ConstantTest) {
    auto f = TFunction::construct(FunctionType::constant, 3);
    ASSERT_TRUE(std::stod(f->toString()) == 3.0);
}

TEST(ConstructAndStringTests, PowerTest) {
    auto f = TFunction::construct(FunctionType::power, 3);
    ASSERT_TRUE(f->toString() == "x^3.000000");
}

TEST(ConstructAndStringTests, ExponentialTest) {
    auto f = TFunction::construct(FunctionType::exponential, 3);
    ASSERT_TRUE(f->toString() == "3.000000^x");
}

TEST(ConstructAndStringTests, PolynomialTest) {
    auto f = TFunction::construct(FunctionType::polynomial, {1, 2, 3});
    ASSERT_TRUE(f->toString() == "1.000000x^0 + 2.000000x^1 + 3.000000x^2");
}

TEST(ValueTests, IdenticalTest) {
    auto f = TFunction::construct(FunctionType::identical);
    auto x = [](double arg){return arg;};
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x(i));
    }
}
TEST(ValueTests, ConstantTest) {
    auto f = TFunction::construct(FunctionType::constant, 3);
    auto x = [](double arg){return 3.0;};
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x(i));
    }
}
TEST(ValueTests, PowerTest) {
    auto f = TFunction::construct(FunctionType::power, 3);
    auto x = [](double arg){return pow(arg, 3);};
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x(i));
    }
}
TEST(ValueTests, ExponentialTest) {
    auto f = TFunction::construct(FunctionType::exponential, 3);
    auto x = [](double arg){return pow(3, arg);};
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x(i));
    }
}
TEST(ValueTests, PolynomialTest) {
    auto f = TFunction::construct(FunctionType::polynomial, {1, 2, 3});
    auto x = [](double arg){return 1 + 2 * arg + 3 * pow(arg, 2);};
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x(i));
    }
}

struct OperationTests : public testing::Test {
    void SetUp() { 
        f1 = TFunction::construct(FunctionType::polynomial, {1, 2, 3});
        f2 = TFunction::construct(FunctionType::exponential, 3);
        x1 = [](double arg){return 1 + 2 * arg + 3 * pow(arg, 2);};
        x2 = [](double arg){return pow(3, arg);};
    }
    void TearDown() {}
    std::shared_ptr<TFunction> f1;
    std::shared_ptr<TFunction> f2;
    std::function<double(double arg)> x1;
    std::function<double(double arg)> x2;
};

TEST_F(OperationTests, AddTest) {
    auto f = f1 + f2;
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x1(i) + x2(i));
    }
}

TEST_F(OperationTests, SubTest) {
    auto f = f1 - f2;
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x1(i) - x2(i));
    }
}

TEST_F(OperationTests, MulTest) {
    auto f = f1 * f2;
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x1(i) * x2(i));
    }
}

TEST_F(OperationTests, DivTest) {
    auto f = f1 / f2;
    for (int i = 0; i < 10; ++i) {
        ASSERT_TRUE(f->value(i) == x1(i) / x2(i));
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
