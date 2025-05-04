#include "BigUint.h"
#include <gtest/gtest.h>

class BigUintTestAccessor {
public:
    static const std::vector<BigUint::DigitType> & get_digits(const BigUint &bigUint) {
        return bigUint.digits_;
    }
};

//---------- Construction test begin-----------//

TEST(BigUintTest, default_constructor_creates_biguint_zero) {
    const BigUint defaultBigUint;
    EXPECT_EQ(defaultBigUint, BigUint::ZERO);
}

TEST(BigUintTest, construction_from_digit_type) {
    constexpr auto digitZero = static_cast<BigUint::DigitType>(0);
    const BigUint fromDigitZero(digitZero);
    EXPECT_EQ(fromDigitZero, BigUint::ZERO);

    constexpr auto digitOne = static_cast<BigUint::DigitType>(1);
    const BigUint fromDigitOne(digitOne);
    EXPECT_EQ(fromDigitOne, BigUint::ONE);

    constexpr auto baseMinusOne = BigUint::BASE - 1;
    const BigUint fromBaseMinusOne(baseMinusOne);
    const auto fromBaseMinusOneDigits = BigUintTestAccessor::get_digits(fromBaseMinusOne);
    EXPECT_EQ(fromBaseMinusOneDigits.size(), 1);
    EXPECT_EQ(fromBaseMinusOneDigits.front(), 65'535);
}

TEST(BigUintTest, construction_from_string_and_digit_vector) {
    const BigUint fromEmptyString("");
    EXPECT_EQ(fromEmptyString, BigUint::ZERO);
    const BigUint fromEmptyVector{std::vector<BigUint::DigitType>()};
    EXPECT_EQ(fromEmptyVector, BigUint::ZERO);

    const BigUint fromStringWithZero("0");
    EXPECT_EQ(fromStringWithZero, BigUint::ZERO);
    const BigUint fromVectorWithZero{std::vector<BigUint::DigitType>(1, static_cast<BigUint::DigitType>(0))};
    EXPECT_EQ(fromVectorWithZero, BigUint::ZERO);

    const BigUint fromStringWithOne("1");
    EXPECT_EQ(fromStringWithOne, BigUint::ONE);
    const BigUint fromVectorWithOne{std::vector<BigUint::DigitType>(1, static_cast<BigUint::DigitType>(1))};
    EXPECT_EQ(fromVectorWithOne, BigUint::ONE);

    const BigUint fromStringWithBaseMinusOne("65535");
    const auto fromStringWithBaseMinusOneDigits = BigUintTestAccessor::get_digits(fromStringWithBaseMinusOne);
    EXPECT_EQ(fromStringWithBaseMinusOneDigits.size(), 1);
    EXPECT_EQ(fromStringWithBaseMinusOneDigits.front(), 65'535);

    const BigUint fromVectorWithBaseMinusOne{std::vector<BigUint::DigitType>(1, static_cast<BigUint::DigitType>(65'535))};
    const auto fromVectorWithBaseMinusOneDigits = BigUintTestAccessor::get_digits(fromVectorWithBaseMinusOne);
    EXPECT_EQ(fromVectorWithBaseMinusOneDigits.size(), 1);
    EXPECT_EQ(fromVectorWithBaseMinusOneDigits.front(), 65'535);

    const BigUint fromStringWithSeventyThousand("1|4464"); // Decimal: 70'000
    const auto fromStringWithSeventyThousandDigits = BigUintTestAccessor::get_digits(fromStringWithSeventyThousand);
    EXPECT_EQ(fromStringWithSeventyThousandDigits.size(), 2);
    EXPECT_EQ(fromStringWithSeventyThousandDigits.front(), 4'464);
    EXPECT_EQ(fromStringWithSeventyThousandDigits.back(), 1);
    constexpr BigUint::WideDigitType asDecimal = 1 * BigUint::BASE + 4'464;
    EXPECT_EQ(asDecimal, static_cast<BigUint::WideDigitType>(70'000));
}

//---------- Construction tests end -----------//


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

TEST(BigUintTest, add_digit_zero) {
    const BigUint a("1|0");
    constexpr auto digitZero = static_cast<BigUint::DigitType>(0);
    const auto aPlus0 = a.add(digitZero);
    EXPECT_EQ(aPlus0, a);
}

TEST(BigUintTest, add_digit_one) {
    const BigUint a("1|0");
    constexpr auto digitOne = static_cast<BigUint::DigitType>(1);
    const auto aPlus1 = a.add(digitOne);
    const BigUint expectedResult("1|1");
    EXPECT_EQ(aPlus1, expectedResult);
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

TEST(BigUintTest, divide_by_ten) {
    const BigUint dividend("1|4464"); // Decimal 70'000
    auto [quotient, remainder] = dividend.divideBy(BigUint(10));
    EXPECT_EQ(quotient, BigUint{7'000});
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, to_base_10_string) {
    EXPECT_EQ(BigUint::ZERO.toBase10String(), "0");
    EXPECT_EQ(BigUint::ONE.toBase10String(), "1");
    EXPECT_EQ(BigUint::TWO.toBase10String(), "2");
    EXPECT_EQ(BigUint::TEN.toBase10String(), "10");
    constexpr BigUint::WideDigit baseWideDigit = BigUint::BASE;
    constexpr auto baseMinusOneDigit = static_cast<BigUint::Digit>(baseWideDigit - 1);
    const BigUint baseMinusOne(baseMinusOneDigit);
    EXPECT_EQ(baseMinusOne.toBase10String(), "65535");
    const BigUint base = baseMinusOne + 1;
    EXPECT_EQ(base.toBase10String(), "65536");
    EXPECT_EQ(BigUint("1|4464").toBase10String(), "70000");
    EXPECT_EQ(BigUint("1|9232|4352").toBase10String(), "4900000000");
}

/*


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
