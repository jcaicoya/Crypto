#ifndef BIGINT_H
#define BIGINT_H

#include <string>
#include <vector>
#include <iostream>

class BigInt {
public:
    // Constructors
    BigInt();
    explicit BigInt(const std::string& value);
    BigInt(long long value);

    // Arithmetic operations
    BigInt operator+(const BigInt& other) const;
    BigInt operator-(const BigInt& other) const;
    BigInt operator*(const BigInt& other) const;
    BigInt operator/(const BigInt& other) const;
    BigInt operator%(const BigInt& other) const;
    BigInt operator-() const;
    BigInt& operator+=(const BigInt& other);
    BigInt& operator-=(const BigInt& other);
    BigInt& operator*=(const BigInt& other);
    BigInt& operator/=(const BigInt& other);
    BigInt& operator%=(const BigInt& other);

    // Modular aritmethic operations
    [[nodiscard]] BigInt modAdd(const BigInt& other, const BigInt& mod) const;
    [[nodiscard]] BigInt modSub(const BigInt& other, const BigInt& mod) const;
    [[nodiscard]] BigInt modMul(const BigInt& other, const BigInt& mod) const;
    [[nodiscard]] BigInt modPow(const BigInt& other, const BigInt& mod) const;

    // Comparison operators
    bool operator<(const BigInt& other) const;
    bool operator<=(const BigInt& other) const;
    bool operator>(const BigInt& other) const;
    bool operator>=(const BigInt& other) const;
    bool operator==(const BigInt& other) const;
    bool operator!=(const BigInt& other) const;

    // Input/output operators
    friend std::ostream& operator<<(std::ostream& os, const BigInt& bigint);
    friend std::istream& operator>>(std::istream& is, BigInt& bigint);

    // Utility functions
    [[nodiscard]] std::string toString() const;
    [[nodiscard]] BigInt abs() const;

private:
    // Internal representation
    std::vector<int> digits; // Digits stored in reverse order for easier arithmetic
    bool isNegative;

    // Helper functions
    void removeLeadingZeros();
    [[nodiscard]] BigInt multiplyClassic(const BigInt& other) const;
    [[nodiscard]] BigInt multiplyFFT(const BigInt& other) const;
};

#endif // BIGINT_H
