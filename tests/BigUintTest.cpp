#include "BigUint.h"
#include <gtest/gtest.h>

class BigUintTestAccessor {
public:
    static const std::vector<BigUint::DigitType> & get_digits(const BigUint &bigUint) {
        return bigUint.digits_;
    }
};

TEST(BigUintTest, default_constructor_creates_zero) {
    BigUint a;
    const auto &digits = BigUintTestAccessor::get_digits(a);
    EXPECT_EQ(digits.size(), 1);
    EXPECT_EQ(digits.front(), 0);
}

TEST(BigUintTest, shift_zero_left_five_positions) {
    BigUint a = BigUint::ZERO;
    a.shiftMeLeft(5);
    EXPECT_EQ(a, BigUint::ZERO);
}

TEST(BigUintTest, shift_one_left_zero_positions) {
    BigUint a = BigUint::ONE;
    a.shiftMeLeft(0);
    EXPECT_EQ(a, BigUint::ONE);
}

TEST(BigUintTest, shift_one_left_five_positions) {
    BigUint a = BigUint::ONE;
    a.shiftMeLeft(5);
    const BigUint expectedResult("1|0|0|0|0|0");
    EXPECT_EQ(a, expectedResult);
}

TEST(BigUintTest, plus_one) {
    BigUint a;
    constexpr int32_t times = BigUint::BASE - 1;
    for (int32_t ii=0; ii<times; ii++) {
        a.mePlusOne();
    }
    EXPECT_EQ(a.toString(), "65535");
    a.mePlusOne();
    EXPECT_EQ(a.toString(), "1|0");
}

TEST(BigUintTest, minus_one_over_zero_launches_runtime_error) {
    BigUint a = BigUint::ZERO;
    EXPECT_THROW(a.meMinusOne(), std::runtime_error);
}

TEST(BigUintTest, minus_one) {
    std::vector<BigUint::DigitType> twoDigits{1, 0};

    BigUint a(twoDigits);
    EXPECT_EQ(a.toString(), "1|0");
    a.meMinusOne();
    EXPECT_EQ(a.toString(), "65535");

    constexpr int32_t times = BigUint::BASE - 1;
    for (int32_t ii=0; ii<times; ii++) {
        a.meMinusOne();
    }
    EXPECT_EQ(a.toString(), "0");
}

TEST(BigUintTest, string_constructions) {
    BigUint a("");
    EXPECT_EQ(a, BigUint::ZERO);

    BigUint b("65535");
    EXPECT_EQ(b.toString(), "65535");

    BigUint c("1|0");
    b.mePlusOne();
    EXPECT_EQ(b, c);
}

TEST(BigUintTest, multiply_by_digit_zero) {
    const BigUint a("1|5");
    const auto aBy0 = a.multiplyByOneDigit(0);
    EXPECT_EQ(aBy0, BigUint::ZERO);
}

TEST(BigUintTest, multiply_by_digit_one) {
    const BigUint a("1|5");
    const BigUint aBy1 = a.multiplyByOneDigit(1);
    EXPECT_EQ(aBy1, a);
}

TEST(BigUintTest, multiply_by_digit_two) {
    const BigUint a("1|5");
    const BigUint aBy2 = a.multiplyByOneDigit(2);
    EXPECT_EQ(aBy2, BigUint("2|10"));
}

TEST(BigUintTest, multiply_by_digit_one_thousand_and_one) {
    const BigUint a("1|5");
    const BigUint aBy1001 = a.multiplyByOneDigit(1'001);
    EXPECT_EQ(aBy1001, BigUint("1001|5005"));
}

TEST(BigUintTest, multiply_by_digit_base_minus_one) {
    const BigUint baseMinusOne = BigUint("65535");
    const BigUint square = baseMinusOne.multiplyByOneDigit(65535); // Decimal: 4'294'836'225
    EXPECT_EQ(square, BigUint("65534|1")); // 65'534 * 65'536 + 1 = 4'294'836'225
}

TEST(BigUintTest, multiply_by_digit_four_thousand_for_hundred_and_sixty_four) {
    const BigUint seventyThousand("1|4464"); // Decimal: 70'000
    const auto result = seventyThousand.multiplyByOneDigit(4464); // Decimal: 312'480'000
    const BigUint expected("4768|4352"); // 4768 * 65536 + 4352 = 312'480'000
    EXPECT_EQ(result, expected);
}

TEST(BigUintTest, multiply_by_zero) {
    const BigUint a("1|4464"); // Decimal 70'000
    const BigUint aBy0 = a * BigUint::ZERO;
    EXPECT_EQ(aBy0, BigUint::ZERO);
}

TEST(BigUintTest, multiply_by_one) {
    const BigUint a("1|4464"); // Decimal 70'000
    const BigUint aBy1 = a.multiplyBy(BigUint::ONE);
    EXPECT_EQ(aBy1, a);
}

TEST(BigUintTest, multiply_by_itself) {
    const BigUint a("1|4464"); // Decimal 70'000
    const BigUint square = a.multiplyBy(a);
    EXPECT_EQ(square, BigUint("1|9232|4352"));
}

TEST(BigUintTest, divide_by_one) {
    const BigUint dividend("1|4464"); // Decimal 70'000
    auto [quotient, remainder] = dividend.divideBy(BigUint::ONE);
    EXPECT_EQ(quotient, dividend);
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, divide_by_itself) {
    const BigUint dividend("1|4464"); // Decimal 70'000
    auto [quotient, remainder] = dividend.divideBy(dividend);
    EXPECT_EQ(quotient, BigUint::ONE);
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, divide_by_itself_plus_one) {
    const BigUint dividend("1|4464"); // Decimal 70'000
    const BigUint divisor = dividend.plusOne();
    const auto [quotient, remainder] = dividend.divideBy(divisor);
    EXPECT_EQ(quotient, BigUint::ZERO);
    EXPECT_EQ(remainder, dividend);
}

TEST(BigUintTest, divide_by_two) {
    const BigUint dividend("1|4464"); // Decimal 70'000
    const auto [quotient, remainder] = dividend.divideBy(BigUint::TWO);
    EXPECT_EQ(quotient, BigUint(35'000));
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, divide_by_three) {
    const BigUint dividend("1|4464"); // Decimal 70'000
    auto [quotient, remainder] = dividend.divideBy(BigUint(3));
    EXPECT_EQ(quotient, BigUint{23'333});
    EXPECT_EQ(remainder, BigUint::ONE);
}

/*
TEST(BigUintTest, Addition) {
    BigUint a(123);
    BigUint b(456);
    BigUint c = a + b;
    EXPECT_EQ(c.toString(), "579");
}

TEST(BigUintTest, Subtraction) {
    BigUint a(1000);
    BigUint b(1);
    BigUint c = a - b;
    EXPECT_EQ(c.toString(), "999");
}

TEST(BigUintTest, Multiplication) {
    BigUint a("123456789");
    BigUint b("987654321");
    BigUint result = a * b;
    EXPECT_EQ(result.toString(), "121932631112635269");
}

TEST(BigUintTest, ModularAddition) {
    BigUint a(12345);
    BigUint b("67890");
    BigUint mod("100000");
    EXPECT_EQ(a.modAdd(b, mod).toString(), "80235");
}

TEST(BigUintTest, ModularExponentiation) {
    BigUint base(5);
    BigUint exponent(5);
    BigUint mod(13);

    EXPECT_EQ(base.modPow(exponent, mod).toBase10String(), "8");  // ✅ 5^3 % 13 = 8

    // ✅ Test large exponentiation
    BigUint bigBase("123456789");
    BigUint bigExp(100);
    BigUint bigMod("987654321");

    BigUint bigResult = bigBase.modPow(bigExp, bigMod);
    EXPECT_EQ(bigResult.toBase10String(), "49744705");  // ✅ Expected result (precomputed)

    // ✅ Edge cases
    EXPECT_EQ(BigUint(7).modPow(BigUint(0), BigUint(100)).toBase10String(), "1");  // ✅ x^0 % y = 1
    EXPECT_EQ(BigUint(2).modPow(BigUint(10), BigUint(1024)).toBase10String(), "0"); // ✅ x^y % x^y = 0
}

TEST(BigUintTest, CompoundAddition) {
    BigUint a(1000);
    BigUint b(500);
    a += b;
    EXPECT_EQ(a.toString(), "1500");
}

TEST(BigUintTest, CompoundSubtraction) {
    BigUint a(1000);
    BigUint b(500);
    a -= b;
    EXPECT_EQ(a.toString(), "500");
}

TEST(BigUintTest, CompoundMultiplication) {
    BigUint a(1000);
    BigUint b(10);
    a *= b;
    EXPECT_EQ(a.toString(), "10000");
}

TEST(BigUintTest, CompoundDivision) {
    BigUint a(1000);
    BigUint b(10);
    a /= b;
    EXPECT_EQ(a.toString(), "100");
}

TEST(BigUintTest, CompoundModulus) {
    BigUint a(1000);
    BigUint b(300);
    a %= b;
    EXPECT_EQ(a.toString(), "100");
}

TEST(BigUintTest, NaiveMultiplication) {
    BigUint a("123456789");
    BigUint b("987654321");
    BigUint result = a.multiplyNaive(b);
    EXPECT_EQ(result.toString(), "121932631112635269");
}

TEST(BigUintTest, KaratsubaMultiplication) {
    BigUint a("123456789");
    BigUint b("987654321");
    BigUint result = a .multiplyFFT(b);
    EXPECT_EQ(result.toString(), "121932631112635269");
}

TEST(BigUintTest, FFTMultiplication) {
    BigUint a("123456789");
    BigUint b("987654321");
    BigUint result = a.multiplyFFT(b);
    EXPECT_EQ(result.toString(), "121932631112635269");
}
*/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
