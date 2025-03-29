#ifndef BigUint_H
#define BigUint_H

#include <string>
#include <vector>
#include <iostream>
#include <cstdint>


class BigUint {
public:
    using DigitType = uint16_t;
    using WideDigitType = uint32_t;
    static constexpr WideDigitType BASE = std::numeric_limits<DigitType>::max() + 1;

    BigUint();
    explicit BigUint(DigitType value);
    explicit BigUint(const std::string& value);
    explicit BigUint(const std::vector<DigitType> &digits);

    BigUint(const BigUint &rhs) = default;
    BigUint & operator=(const BigUint &rhs) = default;
    BigUint(BigUint &&rhs) = default;
    BigUint & operator=(BigUint &&rhs) = default;
    virtual ~BigUint() = default;

    static const BigUint ZERO;
    static const BigUint ONE;
    static const BigUint TWO;
    static const BigUint TEN;

    [[nodiscard]] std::string toString() const;
    BigUint fromString(const std::string &input);

    friend std::ostream& operator<<(std::ostream& os, const BigUint& bigUint);
    friend std::istream& operator>>(std::istream& is, BigUint& bigUint);

    std::strong_ordering operator<=>(const BigUint& other) const;
    bool operator==(const BigUint& other) const = default;  // optional if using spaceship

    BigUint shiftMeLeft(size_t shiftPositions);
    [[nodiscard]] BigUint shiftLeft(size_t shiftPositions) const;

    BigUint mePlusOne();
    [[nodiscard]] BigUint plusOne() const;

    BigUint meMinusOne();
    [[nodiscard]] BigUint minusOne() const;

    BigUint addToMe(const BigUint &rhs);
    [[nodiscard]] BigUint add(const BigUint &rhs) const;
    BigUint operator+=(const BigUint &rhs);
    [[nodiscard]] BigUint operator+(const BigUint &rhs);

    BigUint subtractToMe(const BigUint &rhs);
    [[nodiscard]] BigUint subtract(const BigUint &rhs) const;
    BigUint operator-=(const BigUint &rhs);
    [[nodiscard]] BigUint operator-(const BigUint &rhs) const;

    BigUint multiplyMeByOneDigit(DigitType digit);
    [[nodiscard]] BigUint multiplyByOneDigit(DigitType digit) const;
    BigUint operator*=(DigitType digit);
    [[nodiscard]] BigUint operator*(DigitType digit) const;

    BigUint multiplyMeBy(const BigUint &rhs);
    [[nodiscard]] BigUint multiplyBy(const BigUint &rhs) const;
    BigUint operator*=(const BigUint &rhs);
    [[nodiscard]] BigUint operator*(const BigUint &rhs) const;

    // returns the remainder
    BigUint::DigitType divideMeByOneDigit(DigitType digit);
    // returns quotient and remainder
    [[nodiscard]] std::pair<BigUint, BigUint::DigitType> divideByOneDigit(DigitType digit) const;
    // returns the quotient
    BigUint operator/=(DigitType digit);
    // returns the quotient
    [[nodiscard]] BigUint operator/(DigitType digit) const;
    // returns the remainder
    BigUint::DigitType operator%=(DigitType digit);
    // returns the remainder
    [[nodiscard]] BigUint::DigitType operator%(DigitType) const;

    // returns the remainder
    BigUint divideMeBy(const BigUint &rhs);
    // returns quotient and remainder
    [[nodiscard]] std::pair<BigUint, BigUint> divideBy(const BigUint &rhsr) const;
    // returns the quotient
    BigUint operator/=(const BigUint &rhs);
    // returns the quotient
    [[nodiscard]] BigUint operator/(const BigUint &rhs) const;
    // returns the remainder
    BigUint operator%=(const BigUint &rhs);
    // returns the remainder
    [[nodiscard]] BigUint operator%(const BigUint &rhs) const;

    /*
     *void fromBase10String(const std::string &input);
    [[nodiscard]] std::string toBase10String() const;
    */

    /*
    [[nodiscard]] BigUint modAdd(const BigUint& other, const BigUint& mod) const;
    [[nodiscard]] BigUint modSub(const BigUint& other, const BigUint& mod) const;
    [[nodiscard]] BigUint modMul(const BigUint& other, const BigUint& mod) const;
    [[nodiscard]] BigUint modPow(const BigUint& other, const BigUint& mod) const;
    */

    friend class BigUintTestAccessor;
    friend class BigUintBenchmarkAccessor;

private:
    std::vector<DigitType> digits_; // Digits stored in reverse order for easier arithmetic

    void removeLeadingZeros();
    [[nodiscard]] BigUint multiplyNaive(const BigUint& other) const;
    static std::pair<BigUint, BigUint> divide(const BigUint &dividend, const BigUint &divisor);

    /* Multiplications
    [[nodiscard]] BigUint multiplyFFT(const BigUint& other) const;
    [[nodiscard]] std::pair<BigUint, BigUint> split(size_t pos) const;
    [[nodiscard]] BigUint multiplyKaratsuba(const BigUint& other) const;
    */
};

#endif // BigUint_H
