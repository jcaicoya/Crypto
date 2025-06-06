#include "BigUint.h"
#include <gtest/gtest.h>

class BigUintTestAccessor {
public:
    static const std::vector<BigUint::DigitType> & get_digits(const BigUint &bigUint) {
        return bigUint.digits_;
    }

    static [[nodiscard]] BigUint multiplyNaive(const BigUint &lhs, const BigUint& rhs) {
        return lhs.multiply_me_naive(rhs);
    }

    static [[nodiscard]] BigUint multiplyKaratsuba(const BigUint &lhs, const BigUint& rhs) {
        return lhs.multiply_me_karatsuba(rhs);
    }

    static [[nodiscard]] BigUint multiplyFFT(const BigUint &lhs, const BigUint& rhs) {
        return lhs.multiply_me_fft(rhs);
    }
};

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

TEST(BigUintTest, construction_from_wide_digit_type) {
    constexpr auto baseMinusOne = BigUint::BASE - 1;
    const BigUint fromBaseMinusOne(baseMinusOne);
    const auto fromBaseMinusOneDigits = BigUintTestAccessor::get_digits(fromBaseMinusOne);
    EXPECT_EQ(fromBaseMinusOneDigits.size(), 1);
    EXPECT_EQ(fromBaseMinusOneDigits.front(), 65'535);

    constexpr auto base = BigUint::BASE;
    const BigUint fromBase(base);
    const auto fromBaseDigits = BigUintTestAccessor::get_digits(fromBase);
    ASSERT_EQ(fromBaseDigits.size(), 2);
    EXPECT_EQ(fromBaseDigits.front(), 0);
    EXPECT_EQ(fromBaseDigits.back(), 1);

    constexpr auto basePlusOne = BigUint::BASE + 1;
    const BigUint fromBasePlusOne(basePlusOne);
    const auto fromBasePlusOneDigits = BigUintTestAccessor::get_digits(fromBasePlusOne);
    ASSERT_EQ(fromBaseDigits.size(), 2);
    EXPECT_EQ(fromBasePlusOneDigits.front(), 1);
    EXPECT_EQ(fromBasePlusOneDigits.back(), 1);

    constexpr auto maxWideDigit = static_cast<BigUint::WideDigitType>(4'294'967'295);
    const BigUint fromMaxWideDigit(maxWideDigit);
    const auto fromMaxWideDigits = BigUintTestAccessor::get_digits(fromMaxWideDigit);
    ASSERT_EQ(fromMaxWideDigits.size(), 2);
    EXPECT_EQ(fromMaxWideDigits.front(), 65'535);
    EXPECT_EQ(fromMaxWideDigits.back(), 65'535);
}

TEST(BigUintTest, construction_from_bite_type) {
    constexpr auto digitZero = static_cast<BigUint::ByteType>(0);
    const BigUint fromDigitZero(digitZero);
    EXPECT_EQ(fromDigitZero, BigUint::ZERO);

    constexpr auto digitOne = static_cast<BigUint::ByteType>(1);
    const BigUint fromDigitOne(digitOne);
    EXPECT_EQ(fromDigitOne, BigUint::ONE);

    constexpr auto maxByteType = static_cast<BigUint::ByteType>(255);
    const BigUint fromMaxByteType(maxByteType);
    const auto fromMaxByteTypeDigits = BigUintTestAccessor::get_digits(fromMaxByteType);
    EXPECT_EQ(fromMaxByteTypeDigits.size(), 1);
    EXPECT_EQ(fromMaxByteTypeDigits.front(), 255);
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

TEST(BigUintTest, check_least_significat_digit) {
    const BigUint value = BigUint::from_base10_string("70000");
    EXPECT_EQ(value.get_least_significant_digit(), static_cast<BigUint::DigitType>(4464));
}

TEST(BigUintTest, check_most_significat_digit) {
    const BigUint value = BigUint::from_base10_string("70000");
    EXPECT_EQ(value.get_most_significant_digit(), static_cast<BigUint::DigitType>(1));
}

TEST(BigUintTest, shift_zero_left_five_positions) {
    BigUint a = BigUint::ZERO;
    a.shift_me_left(5);
    EXPECT_EQ(a, BigUint::ZERO);
}

TEST(BigUintTest, shift_one_left_zero_positions) {
    BigUint a = BigUint::ONE;
    a.shift_me_left(0);
    EXPECT_EQ(a, BigUint::ONE);
}

TEST(BigUintTest, shift_one_left_five_positions) {
    BigUint a = BigUint::ONE;
    a.shift_me_left(5);
    const BigUint expected_result("1|0|0|0|0|0");
    EXPECT_EQ(a, expected_result);
}

TEST(BigUintTest, plus_one) {
    BigUint a = BigUint::ZERO;
    constexpr int32_t times = BigUint::BASE - 1;
    for (int32_t ii=0; ii<times; ii++) {
        a.me_plus_one();
    }
    EXPECT_EQ(a.to_string(), "65535");
    a.me_plus_one();
    EXPECT_EQ(a.to_string(), "1|0");
}

TEST(BigUintTest, minus_one) {
    EXPECT_THROW(BigUint(0).me_minus_one(), std::runtime_error);

    BigUint a(BigUint::BASE);
    a.me_minus_one();
    EXPECT_EQ(a.as_digit(), 65'535);

    constexpr int32_t times = BigUint::BASE - 1;
    for (int32_t ii=0; ii<times; ii++) {
        a.me_minus_one();
    }
    EXPECT_EQ(a.to_string(), "0");
}

TEST(BigUintTest, add_one_digit) {
    const BigUint a(BigUint::BASE);
    const auto a_plus_0 = a.add(0);
    EXPECT_EQ(a_plus_0, a);

    const auto a_plus_1 = a.add(1);
    const BigUint expected_result("1|1");
    EXPECT_EQ(a_plus_1, expected_result);
}

TEST(BigUintTest, add_big_number) {
    const BigUint a("65535|65535");
    const BigUint b("1|1");
    BigUint obtained_result = a + b;
    BigUint expected_result("1|1|0");
    EXPECT_EQ(obtained_result, expected_result);

    const BigUint large_a("65535|65535|65535");
    obtained_result = large_a + b;
    expected_result = BigUint("1|0|1|0");
    EXPECT_EQ(obtained_result, expected_result);

    obtained_result = b + large_a;
    EXPECT_EQ(obtained_result, expected_result);
}

TEST(BigUintTest, subtract_one_digit) {
    const BigUint a("1|1");
    const auto a_minus_0 = a.subtract(0);
    EXPECT_EQ(a_minus_0, a);

    const auto a_minus_1 = a.subtract(1);
    BigUint expected_result("1|0");
    EXPECT_EQ(a_minus_1, expected_result);

    const auto a_minus_2 = a_minus_1.subtract(1);
    expected_result = BigUint(65535);
    EXPECT_EQ(a_minus_2, expected_result);

    BigUint negative_result;
    EXPECT_THROW(negative_result = BigUint(33333) - 65535, std::runtime_error);
}

TEST(BigUintTest, subtract_big_number) {
    const BigUint a("1|65535|65534");
    const BigUint b("65535|65535");
    BigUint obtained_result = a - b;
    BigUint expected_result("65535|65535");
    EXPECT_EQ(obtained_result, expected_result);

    const BigUint c("65535|65535|65535");
    obtained_result = c - b;
    expected_result = BigUint("65534|65535|0");
    EXPECT_EQ(obtained_result, expected_result);

    EXPECT_THROW(obtained_result = b - c;, std::runtime_error);
}

TEST(BigUintTest, multiply_by_digit_zero) {
    const BigUint a(70'000);
    const auto a_by_0 = a.multiply_by(0);
    EXPECT_EQ(a_by_0, BigUint::ZERO);
}

TEST(BigUintTest, multiply_by_digit_one) {
    const BigUint a(70'000);
    const BigUint a_by_1 = a.multiply_by(1);
    EXPECT_EQ(a_by_1, a);
}

TEST(BigUintTest, multiply_by_digit_two) {
    const BigUint a(70'000);
    const BigUint a_by_2 = a.multiply_by(2);
    EXPECT_EQ(a_by_2, BigUint(140'000));
}

TEST(BigUintTest, multiply_by_digit_one_thousand_and_one) {
    const BigUint a(70'000);
    const BigUint a_by_1001 = a.multiply_by(1'001);
    EXPECT_EQ(a_by_1001, BigUint(70'070'000));
}

TEST(BigUintTest, multiply_by_digit_base_minus_one) {
    const BigUint base_minus_one = BigUint(65535);
    const BigUint square = base_minus_one.multiply_by(65535); // Decimal: 4'294'836'225
    EXPECT_EQ(square, BigUint("65534|1")); // 65'534 * 65'536 + 1 = 4'294'836'225
}

TEST(BigUintTest, multiply_by_digit_four_thousand_for_hundred_and_sixty_four) {
    const BigUint a(70'000);
    const auto result = a.multiply_by(4464); // Decimal: 312'480'000
    const BigUint expected("4768|4352"); // 4768 * 65536 + 4352 = 312'480'000
    EXPECT_EQ(result, expected);
}

TEST(BigUintTest, multiply_by_zero) {
    const BigUint a(70'000);
    const BigUint a_by_0 = a * BigUint::ZERO;
    EXPECT_EQ(a_by_0, BigUint::ZERO);
}

TEST(BigUintTest, multiply_by_one) {
    const BigUint a(70'000);
    const BigUint a_by_1 = a.multiply_by(BigUint::ONE);
    EXPECT_EQ(a_by_1, a);
}

TEST(BigUintTest, multiply_by_itself) {
    const BigUint a(70'000);
    const BigUint square = a.multiply_by(a);
    EXPECT_EQ(square, BigUint::from_base10_string("4900000000"));
}

TEST(BigUintTest, square) {
    const BigUint zero = BigUint::ZERO;
    BigUint square = zero.square();
    EXPECT_EQ(square, BigUint::ZERO);

    const BigUint one = BigUint::ONE;
    square = one.square();
    EXPECT_EQ(square, BigUint::ONE);

    BigUint a = BigUint::TWO;
    BigUint::Digit expected_result = 2;
    for (int ii=1; ii<=3; ii++) {
       a.square_me();
       expected_result *= expected_result;
       EXPECT_EQ(a, BigUint(expected_result));
    }

    a.square_me();
    EXPECT_EQ(a, BigUint(65'536));

    a = BigUint(70'000);
    square = a.square();
    EXPECT_EQ(square, BigUint::from_base10_string("4900000000"));

    a = BigUint::from_base10_string("12345");
    a.square_me();
    EXPECT_EQ(a.to_base10_string(), "152399025");
}

TEST(BigUintTest, big_square) {
    auto a = BigUint::from_base10_string("5000000000");
    a.square_me();
    const auto expected_result = BigUint::from_base10_string("25000000000000000000");
    EXPECT_EQ(a, expected_result);
}

TEST(BigUintTest, pow_by_zero) {
    constexpr BigUint::Digit zero = 0;

    BigUint base = BigUint::ZERO;
    BigUint result;
    EXPECT_THROW(result = base.pow_by(zero), std::runtime_error);

    base = BigUint::ONE;
    result = base.pow_by(zero);
    EXPECT_EQ(result, BigUint::ONE);

    base = BigUint::from_base10_string("5000000000");
    result = base.pow_by(zero);
    EXPECT_EQ(result, BigUint::ONE);
}

TEST(BigUintTest, pow_by_one) {
    constexpr BigUint::Digit one = 1;

    BigUint base = BigUint::ZERO;
    BigUint result = base.pow_by(one);
    EXPECT_EQ(result, BigUint::ONE);

    base = BigUint::ONE;
    result = base.pow_by(one);
    EXPECT_EQ(result, BigUint::ONE);

    base = BigUint::from_base10_string("5000000000");
    result = base.pow_by(one);
    EXPECT_EQ(result, base);
}

TEST(BigUintTest, powers_of_two) {
    const BigUint base = BigUint::TWO;
    BigUint result = base.pow_by(16);
    EXPECT_EQ(result, BigUint::from_base10_string("65536"));

    result = base.pow_by(17);
    EXPECT_EQ(result, BigUint::from_base10_string("131072"));
}

TEST(BigUintTest, powers_of_ten) {
    const BigUint base = BigUint::TEN;
    BigUint result = base.pow_by(16);
    EXPECT_EQ(result, BigUint::from_base10_string("10000000000000000"));

    result = base.pow_by(19);
    EXPECT_EQ(result, BigUint::from_base10_string("10000000000000000000"));
}

TEST(BigUintTest, divide_by_one) {
    const BigUint dividend = BigUint::from_base10_string("70000");
    auto [quotient, remainder] = dividend.divide_by(BigUint::ONE);
    EXPECT_EQ(quotient, dividend);
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, divide_by_itself) {
    const BigUint dividend = BigUint::from_base10_string("70000");
    auto [quotient, remainder] = dividend.divide_by(dividend);
    EXPECT_EQ(quotient, BigUint::ONE);
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, divide_by_itself_plus_one) {
    const BigUint dividend = BigUint::from_base10_string("70000");
    const BigUint divisor = dividend.plus_one();
    const auto [quotient, remainder] = dividend.divide_by(divisor);
    EXPECT_EQ(quotient, BigUint::ZERO);
    EXPECT_EQ(remainder, dividend);
}

TEST(BigUintTest, divide_by_two) {
    const BigUint dividend = BigUint::from_base10_string("70000");
    const auto [quotient, remainder] = dividend.divide_by(BigUint::TWO);
    EXPECT_EQ(quotient, BigUint(35'000));
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, divide_by_three) {
    const BigUint dividend = BigUint::from_base10_string("70000");
    auto [quotient, remainder] = dividend.divide_by(BigUint(3));
    EXPECT_EQ(quotient, BigUint{23'333});
    EXPECT_EQ(remainder, BigUint::ONE);
}

TEST(BigUintTest, divide_by_ten) {
    const BigUint dividend = BigUint::from_base10_string("70000");
    auto [quotient, remainder] = dividend.divide_by(BigUint(10));
    EXPECT_EQ(quotient, BigUint{7'000});
    EXPECT_EQ(remainder, BigUint::ZERO);
}

TEST(BigUintTest, to_base_10_string) {
    EXPECT_EQ(BigUint::ZERO.to_base10_string(), "0");
    EXPECT_EQ(BigUint::ONE.to_base10_string(), "1");
    EXPECT_EQ(BigUint::TWO.to_base10_string(), "2");
    EXPECT_EQ(BigUint::TEN.to_base10_string(), "10");
    constexpr BigUint::WideDigit baseWideDigit = BigUint::BASE;
    constexpr auto baseMinusOneDigit = static_cast<BigUint::Digit>(baseWideDigit - 1);
    const BigUint baseMinusOne(baseMinusOneDigit);
    EXPECT_EQ(baseMinusOne.to_base10_string(), "65535");
    const BigUint base = baseMinusOne + 1;
    EXPECT_EQ(base.to_base10_string(), "65536");
    EXPECT_EQ(BigUint("1|4464").to_base10_string(), "70000");
    EXPECT_EQ(BigUint("1|9232|4352").to_base10_string(), "4900000000");
}

TEST(BigUintTest, ModularAddition) {
    const BigUint a = BigUint::from_base10_string("12345");
    const BigUint b = BigUint::from_base10_string("67890");
    const BigUint mod = BigUint::from_base10_string("100000");
    const auto result = BigUint::mod_add(a, b, mod);
    const auto expected = BigUint::from_base10_string("80235");
    EXPECT_EQ(result, expected);
}

TEST(BigUintTest, ModularSubtraction) {
    const BigUint a = BigUint::from_base10_string("123456");
    const BigUint b = BigUint::from_base10_string("678901");
    const BigUint mod = BigUint::from_base10_string("100000");
    const auto bModSubAResult = BigUint::mod_sub(b, a, mod);
    auto expected = BigUint::from_base10_string("55445");
    EXPECT_EQ(bModSubAResult, expected);

    const auto aModSubBResult = BigUint::mod_sub(a, b, mod);
    expected = BigUint::from_base10_string("44555");
    EXPECT_EQ(aModSubBResult, expected);
}

TEST(BigUintTest, ModularMultiplication) {
    const BigUint a = BigUint::from_base10_string("12345");
    const BigUint b = BigUint::from_base10_string("67890");
    const BigUint mod = BigUint::from_base10_string("10000");
    const auto result = BigUint::mod_mul(a, b, mod); // multiplication is 838.102.050
    const auto expected = BigUint::from_base10_string("2050");
    EXPECT_EQ(result, expected);
}

/*
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
*/

TEST(BigUintTest, gcd) {
    BigUint a(4096);
    BigUint b(144);
    BigUint g = BigUint::gcd(a, b);
    EXPECT_EQ(g.as_digit(), 16);

    a = BigUint::from_base10_string("12345678901234567890");
    b = BigUint::from_base10_string("11223344556677889900");
    g = BigUint::gcd(a, b);
    EXPECT_EQ(g.as_digit(), 30);

    a = BigUint::from_base10_string("4294967296");
    b = BigUint::from_base10_string("1853020188851841");
    g = BigUint::gcd(a, b);
    EXPECT_EQ(g, BigUint::ONE);
}

TEST(BigUintTest, lcm) {
    BigUint a(4096);
    BigUint b(144);
    BigUint l = BigUint::lcm(a, b);
    EXPECT_EQ(l.as_digit(), 36864);

    a = BigUint::from_base10_string("12345678901234567890");
    b = BigUint::from_base10_string("11223344556677889900");
    l = BigUint::lcm(a, b);
    EXPECT_EQ(l, BigUint::from_base10_string("4618660269822135341434463931449843700"));

    a = BigUint::from_base10_string("4294967296");
    b = BigUint::from_base10_string("1853020188851841");
    l = BigUint::lcm(a, b);
    EXPECT_EQ(l, BigUint::from_base10_string("7958661109946400884391936"));
}

TEST(BigUintTest, NaiveMultiplication) {
    const BigUint a = BigUint::from_base10_string("123456789");
    const BigUint b = BigUint::from_base10_string("987654321");
    const BigUint result = BigUintTestAccessor::multiplyNaive(a, b);
    const BigUint expected = BigUint::from_base10_string("121932631112635269");
    EXPECT_EQ(result, expected);
}

TEST(BigUintTest, KaratsubaMultiplication) {
    const BigUint a = BigUint::from_base10_string("123456789");
    const BigUint b = BigUint::from_base10_string("987654321");
    const BigUint result = BigUintTestAccessor::multiplyFFT(a, b);
    const BigUint expected = BigUint::from_base10_string("121932631112635269");
    EXPECT_EQ(result, expected);
}

TEST(BigUintTest, FFTMultiplication) {
    const BigUint a = BigUint::from_base10_string("123456789");
    const BigUint b = BigUint::from_base10_string("987654321");
    const BigUint result = BigUintTestAccessor::multiplyFFT(a, b);
    const BigUint expected = BigUint::from_base10_string("121932631112635269");
    EXPECT_EQ(result, expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
