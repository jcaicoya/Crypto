#ifndef BigUint_H
#define BigUint_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>
#include <optional>

class BigUint {
public:
    using DigitType = uint16_t;
    using Digit = DigitType;
    using Digits = std::vector<Digit>;
    using WideDigitType = uint32_t;
    using WideDigit = WideDigitType;
    using ByteType = uint8_t;
    using ByteDigit = ByteType;
    static constexpr WideDigitType BASE = std::numeric_limits<DigitType>::max() + 1;

    BigUint(WideDigit digit = 0);
    explicit BigUint(const std::string& str);
    explicit BigUint(const Digits &digits);

    BigUint(const BigUint &rhs) = default;
    BigUint & operator=(const BigUint &rhs) = default;
    BigUint(BigUint &&rhs) = default;
    BigUint & operator=(BigUint &&rhs) = default;
    virtual ~BigUint() = default;

    static const BigUint ZERO;
    static const BigUint ONE;
    static const BigUint TWO;
    static const BigUint TEN;

    [[nodiscard]] std::optional<DigitType> as_digit() const;
    [[nodiscard]] std::optional<WideDigitType> as_wide_digit() const;
    [[nodiscard]] std::optional<ByteType> as_byte_digit() const;

    [[nodiscard]] Digit get_least_significant_digit() const { return digits_.front(); }
    [[nodiscard]] Digit get_most_significant_digit() const { return digits_.back(); }

    void set_zero() { *this = BigUint::ZERO; }
    void set_one() { *this = BigUint::ONE; }
    void set_digits(const Digits &digits);
    [[nodiscard]] const Digits & get_digits() const { return digits_; }

    [[nodiscard]] std::string to_string() const;
    BigUint from_string(const std::string &input);

    friend std::ostream& operator<<(std::ostream& os, const BigUint& bigUint);
    friend std::istream& operator>>(std::istream& is, BigUint& bigUint);

    std::strong_ordering operator<=>(const BigUint& other) const;
    bool operator==(const BigUint& other) const = default;  // optional if using spaceship

    void shift_me_left(size_t shiftPositions);
    [[nodiscard]] BigUint shift_left(size_t shiftPositions) const;

    void me_plus_one();
    [[nodiscard]] BigUint plus_one() const;

    void me_minus_one();
    [[nodiscard]] BigUint minus_one() const;

    void add_to_me(DigitType digit);
    [[nodiscard]] BigUint add(DigitType digit) const;
    void operator+=(DigitType digit);
    [[nodiscard]] BigUint operator+(DigitType digit) const;

    void add_to_me(const BigUint &rhs);
    [[nodiscard]] BigUint add(const BigUint &rhs) const;
    void operator+=(const BigUint &rhs);
    [[nodiscard]] BigUint operator+(const BigUint &rhs) const;

    void subtract_to_me(const BigUint &rhs);
    [[nodiscard]] BigUint subtract(const BigUint &rhs) const;
    void operator-=(const BigUint &rhs);
    [[nodiscard]] BigUint operator-(const BigUint &rhs) const;

    void multiply_me_by(DigitType digit);
    [[nodiscard]] BigUint multiply_by(DigitType digit) const;
    void operator*=(DigitType digit);
    [[nodiscard]] BigUint operator*(DigitType digit) const;

    void multiply_me_by(const BigUint &rhs);
    [[nodiscard]] BigUint multiply_by(const BigUint &rhs) const;
    void operator*=(const BigUint &rhs);
    [[nodiscard]] BigUint operator*(const BigUint &rhs) const;

    void square_me();
    [[nodiscard]] BigUint square() const;

    // returns the remainder
    BigUint::DigitType divide_me_by(DigitType digit);
    // returns quotient and remainder
    [[nodiscard]] std::pair<BigUint, BigUint::DigitType> divide_by(DigitType digit) const;
    // returns the quotient
    BigUint operator/=(DigitType digit);
    // returns the quotient
    [[nodiscard]] BigUint operator/(DigitType digit) const;
    // returns the remainder
    BigUint::DigitType operator%=(DigitType digit);
    // returns the remainder
    [[nodiscard]] BigUint::DigitType operator%(DigitType) const;

    // returns the remainder
    BigUint divide_me_by(const BigUint &rhs);
    // returns quotient and remainder
    [[nodiscard]] std::pair<BigUint, BigUint> divide_by(const BigUint &rhsr) const;
    // returns the quotient
    BigUint operator/=(const BigUint &rhs);
    // returns the quotient
    [[nodiscard]] BigUint operator/(const BigUint &rhs) const;
    // returns the remainder
    BigUint operator%=(const BigUint &rhs);
    // returns the remainder
    [[nodiscard]] BigUint operator%(const BigUint &rhs) const;

    [[nodiscard]] std::string to_base10_string() const;
    static [[nodiscard]] BigUint from_base10_string(const std::string &input);

    static [[nodiscard]] BigUint mod_add(const BigUint& lhs, const BigUint& rhs, const BigUint& mod);
    static [[nodiscard]] BigUint mod_sub(const BigUint& lhs, const BigUint& rhs, const BigUint& mod);
    static [[nodiscard]] BigUint mod_mul(const BigUint& lhs, const BigUint& rhs, const BigUint& mod);
    /*
    [[nodiscard]] BigUint modPow(const BigUint& other, const BigUint& mod) const;
    */

    friend class BigUintTestAccessor;
    friend class BigUintBenchmarkAccessor;

private:
    Digits digits_; // Digits stored in reverse order for easier arithmetic

    void remove_leading_zeros();
    [[nodiscard]] BigUint multiply_me_naive(const BigUint& other) const;
    static std::pair<BigUint, BigUint> divide_by(const BigUint &dividend, const BigUint &divisor);

    // Multiplications
    [[nodiscard]] BigUint multiply_me_fft(const BigUint& other) const;
    [[nodiscard]] std::pair<BigUint, BigUint> split(std::size_t pos) const;
    [[nodiscard]] BigUint multiply_me_karatsuba(const BigUint& other) const;

    // Helpers
    static [[nodiscard]] std::vector<BigUint::DigitType> opt_inner_square(const std::vector<BigUint::DigitType> &digits);
};

std::ostream& operator<<(std::ostream& os, const BigUint& bigUint);
std::istream& operator>>(std::istream& is, BigUint& bigUint);

#endif // BigUint_H
