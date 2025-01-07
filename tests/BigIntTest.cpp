#include "BigInt.h"
#include <gtest/gtest.h>

TEST(BigIntTest, DefaultConstructor) {
    BigInt a;
    EXPECT_EQ(a.toString(), "0");
}

TEST(BigIntTest, StringConstructor) {
    BigInt a("123456");
    EXPECT_EQ(a.toString(), "123456");
}

TEST(BigIntTest, Addition) {
    BigInt a("123");
    BigInt b("456");
    BigInt c = a + b;
    EXPECT_EQ(c.toString(), "579");
}

TEST(BigIntTest, Subtraction) {
    BigInt a("1000");
    BigInt b("1");
    BigInt c = a - b;
    EXPECT_EQ(c.toString(), "999");
}

TEST(BigIntTest, Multiplication) {
    BigInt a("123456789");
    BigInt b("987654321");
    BigInt result = a * b;
    EXPECT_EQ(result.toString(), "121932631112635269");
}

TEST(BigIntTest, ModularAddition) {
    BigInt a("12345");
    BigInt b("67890");
    BigInt mod("100000");
    EXPECT_EQ(a.modAdd(b, mod).toString(), "80235");
}

TEST(BigIntTest, ModularExponentiation) {
    BigInt base("5");
    BigInt exp("3");
    BigInt mod("13");
    EXPECT_EQ(base.modPow(exp, mod).toString(), "8"); // 5^3 % 13 = 125 % 13 = 8
}

TEST(BigIntTest, UnaryMinus) {
    BigInt a("12345");
    BigInt b = -a;
    EXPECT_EQ(b.toString(), "-12345");

    BigInt c("-98765");
    BigInt d = -c;
    EXPECT_EQ(d.toString(), "98765");

    BigInt e(0);
    BigInt f = -e;
    EXPECT_EQ(f.toString(), "0");
}

TEST(BigIntTest, AbsoluteValue) {
    BigInt a("-12345");
    EXPECT_EQ(a.abs().toString(), "12345");

    BigInt b("98765");
    EXPECT_EQ(b.abs().toString(), "98765");

    BigInt c(0);
    EXPECT_EQ(c.abs().toString(), "0");
}

#include "BigInt.h"
#include <gtest/gtest.h>

TEST(BigIntTest, CompoundAddition) {
    BigInt a("1000");
    BigInt b("500");
    a += b;
    EXPECT_EQ(a.toString(), "1500");
}

TEST(BigIntTest, CompoundSubtraction) {
    BigInt a("1000");
    BigInt b("500");
    a -= b;
    EXPECT_EQ(a.toString(), "500");
}

TEST(BigIntTest, CompoundMultiplication) {
    BigInt a("1000");
    BigInt b("10");
    a *= b;
    EXPECT_EQ(a.toString(), "10000");
}

TEST(BigIntTest, CompoundDivision) {
    BigInt a("1000");
    BigInt b("10");
    a /= b;
    EXPECT_EQ(a.toString(), "100");
}

TEST(BigIntTest, CompoundModulus) {
    BigInt a("1000");
    BigInt b("300");
    a %= b;
    EXPECT_EQ(a.toString(), "100");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
