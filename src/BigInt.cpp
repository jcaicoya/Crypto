#include "BigInt.h"
// #include <algorithm>
#include <stdexcept>
#include <complex>
#include <cmath>
#include <vector>

BigInt::BigInt() : isNegative(false) {}

BigInt::BigInt(const std::string& value) {
    if (value.empty()) {
        throw std::invalid_argument("Empty string is not a valid number");
    }

    size_t startIndex = 0;
    isNegative = false;

    // Check for negative sign
    if (value[0] == '-') {
        isNegative = true;
        startIndex = 1;
    } else if (value[0] == '+') {
        startIndex = 1;
    }

    // Parse digits
    for (size_t i = startIndex; i < value.size(); ++i) {
        if (!isdigit(value[i])) {
            throw std::invalid_argument("Invalid character in number string");
        }
        digits.push_back(value[i] - '0'); // Convert char to int
    }

    // Reverse digits to store in reverse order
    std::reverse(digits.begin(), digits.end());

    // Remove leading zeros (except for "0")
    removeLeadingZeros();
}

BigInt::BigInt(long long value) {
    isNegative = (value < 0);
    value = std::abs(value);
    while (value > 0) {
        digits.push_back(value % 10);
        value /= 10;
    }
    if (digits.empty()) digits.push_back(0);
}

void BigInt::removeLeadingZeros() {
    while (digits.size() > 1 && digits.back() == 0) {
        digits.pop_back();
    }
    if (digits.size() == 1 && digits[0] == 0) isNegative = false;
}

std::string BigInt::toString() const {
    if (digits.empty()) return "0";
    std::string result = isNegative ? "-" : "";
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        result += std::to_string(*it);
    }
    return result;
}

BigInt BigInt::abs() const {
    BigInt result = *this;  // Copy the current instance
    result.isNegative = false;  // Make it positive
    return result;
}

std::ostream& operator<<(std::ostream& os, const BigInt& bigint) {
    os << bigint.toString();
    return os;
}

std::istream& operator>>(std::istream& is, BigInt& bigint) {
    std::string value;
    is >> value;
    bigint = BigInt(value);
    return is;
}

BigInt BigInt::operator+(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        // Handle addition of opposite signs as subtraction
        if (isNegative) return other - (-(*this));
        else return *this - (-other);
    }

    BigInt result;
    result.isNegative = isNegative;

    const std::vector<int>& longer = (digits.size() >= other.digits.size()) ? digits : other.digits;
    const std::vector<int>& shorter = (digits.size() < other.digits.size()) ? digits : other.digits;

    int carry = 0;
    for (size_t i = 0; i < longer.size(); ++i) {
        int digitSum = carry;
        if (i < shorter.size()) digitSum += shorter[i];
        digitSum += longer[i];

        result.digits.push_back(digitSum % 10);
        carry = digitSum / 10;
    }

    if (carry > 0) {
        result.digits.push_back(carry);
    }

    return result;
}

BigInt BigInt::operator-(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        // Handle subtraction of opposite signs as addition
        return *this + (-other);
    }

    if (*this == other) {
        return BigInt(0);
    }

    bool resultNegative = false;
    const BigInt* larger = this;
    const BigInt* smaller = &other;

    if (abs() < other.abs()) {
        larger = &other;
        smaller = this;
        resultNegative = !isNegative;
    } else {
        resultNegative = isNegative;
    }

    BigInt result;
    result.isNegative = resultNegative;

    int borrow = 0;
    for (size_t i = 0; i < larger->digits.size(); ++i) {
        int diff = larger->digits[i] - borrow;
        if (i < smaller->digits.size()) {
            diff -= smaller->digits[i];
        }
        if (diff < 0) {
            diff += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.digits.push_back(diff);
    }

    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator*(const BigInt& other) const {
    if (constexpr auto useFFT = true; useFFT) {
        return multiplyFFT(other);
    }
    else {
        return multiplyClassic(other);
    }
}

BigInt BigInt::operator/(const BigInt& other) const {
    if (other == BigInt(0)) {
        throw std::invalid_argument("Division by zero");
    }

    BigInt dividend = this->abs();
    BigInt divisor = other.abs();
    BigInt result;
    result.digits.resize(dividend.digits.size(), 0);

    BigInt current;
    for (int i = static_cast<int>(dividend.digits.size()) - 1; i >= 0; --i) {
        current.digits.insert(current.digits.begin(), dividend.digits[i]);
        current.removeLeadingZeros();

        int count = 0;
        while (current >= divisor) {
            current = current - divisor;
            ++count;
        }
        result.digits[i] = count;
    }

    result.isNegative = (this->isNegative != other.isNegative) && (result != BigInt(0));
    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::operator%(const BigInt& other) const {
    if (other == BigInt(0)) {
        throw std::invalid_argument("Division by zero");
    }
    BigInt remainder = *this - (*this / other) * other;
    if (remainder.isNegative) {
        remainder += other.abs();
    }
    return remainder;
}


BigInt BigInt::operator-() const {
    BigInt result = *this;  // Copy the current instance
    if (result != BigInt(0)) {  // Negate only if it's not zero
        result.isNegative = !result.isNegative;
    }
    return result;
}

BigInt& BigInt::operator+=(const BigInt& other) {
    *this = *this + other; // Use the + operator
    return *this; // Return reference to *this
}

BigInt& BigInt::operator-=(const BigInt& other) {
    *this = *this - other; // Use the - operator
    return *this; // Return reference to *this
}

BigInt& BigInt::operator*=(const BigInt& other) {
    *this = *this * other; // Use the * operator
    return *this; // Return reference to *this
}

BigInt& BigInt::operator/=(const BigInt& other) {
    *this = *this / other; // Use the / operator
    return *this; // Return reference to *this
}

BigInt& BigInt::operator%=(const BigInt& other) {
    *this = *this % other; // Use the % operator
    return *this; // Return reference to *this
}

bool BigInt::operator<(const BigInt& other) const {
    if (isNegative != other.isNegative) {
        return isNegative;
    }

    if (digits.size() != other.digits.size()) {
        return (digits.size() < other.digits.size()) ^ isNegative;
    }

    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        if (digits[i] != other.digits[i]) {
            return (digits[i] < other.digits[i]) ^ isNegative;
        }
    }

    return false;
}

bool BigInt::operator<=(const BigInt& other) const {
    return *this < other || *this == other;
}

bool BigInt::operator>(const BigInt& other) const {
    return !(*this <= other);
}

bool BigInt::operator>=(const BigInt& other) const {
    return !(*this < other);
}

bool BigInt::operator==(const BigInt& other) const {
    return isNegative == other.isNegative && digits == other.digits;
}

bool BigInt::operator!=(const BigInt& other) const {
    return !(*this == other);
}

BigInt BigInt::multiplyClassic(const BigInt& other) const {
    BigInt result;
    result.digits.resize(digits.size() + other.digits.size(), 0);

    for (size_t i = 0; i < digits.size(); ++i) {
        int carry = 0;
        for (size_t j = 0; j < other.digits.size() || carry != 0; ++j) {
            int current = result.digits[i + j] +
                          digits[i] * (j < other.digits.size() ? other.digits[j] : 0) +
                          carry;
            result.digits[i + j] = current % 10;
            carry = current / 10;
        }
    }

    result.isNegative = isNegative != other.isNegative;
    result.removeLeadingZeros();
    return result;
}


using Complex = std::complex<double>;
const double PI = acos(-1);

// Helper function: Perform FFT
void fft(std::vector<Complex>& a, bool invert) {
    size_t n = a.size();
    if (n == 1) return;

    std::vector<Complex> a0(n / 2), a1(n / 2);
    for (size_t i = 0; 2 * i < n; ++i) {
        a0[i] = a[2 * i];
        a1[i] = a[2 * i + 1];
    }

    fft(a0, invert);
    fft(a1, invert);

    double angle = 2 * PI / n * (invert ? -1 : 1);
    Complex w(1), wn(cos(angle), sin(angle));
    for (size_t i = 0; 2 * i < n; ++i) {
        a[i] = a0[i] + w * a1[i];
        a[i + n / 2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2;
            a[i + n / 2] /= 2;
        }
        w *= wn;
    }
}

// Helper function: Multiply using FFT
std::vector<int> multiplyFFTHelper(const std::vector<int>& a, const std::vector<int>& b) {
    std::vector<Complex> fa(a.begin(), a.end()), fb(b.begin(), b.end());
    size_t n = 1;
    while (n < a.size() + b.size()) n <<= 1;
    fa.resize(n);
    fb.resize(n);

    fft(fa, false);
    fft(fb, false);
    for (size_t i = 0; i < n; ++i) {
        fa[i] *= fb[i];
    }
    fft(fa, true);

    std::vector<int> result(n);
    long long carry = 0;
    for (size_t i = 0; i < n; ++i) {
        long long value = static_cast<long long>(round(fa[i].real())) + carry;
        result[i] = value % 10;
        carry = value / 10;
    }

    while (result.size() > 1 && result.back() == 0) {
        result.pop_back();
    }
    return result;
}

// Updated multiplication operator using FFT
BigInt BigInt::multiplyFFT(const BigInt& other) const {
    BigInt result;
    result.digits = multiplyFFTHelper(digits, other.digits);
    result.isNegative = isNegative != other.isNegative;
    result.removeLeadingZeros();
    return result;
}

BigInt BigInt::modAdd(const BigInt& other, const BigInt& mod) const {
    BigInt result = (*this + other) % mod;
    if (result.isNegative) result = result + mod;
    return result;
}

BigInt BigInt::modSub(const BigInt& other, const BigInt& mod) const {
    BigInt result = (*this - other) % mod;
    if (result.isNegative) result = result + mod;
    return result;
}

BigInt BigInt::modMul(const BigInt& other, const BigInt& mod) const {
    BigInt result = (*this * other) % mod;
    if (result.isNegative) result = result + mod;
    return result;
}

BigInt BigInt::modPow(const BigInt& exp, const BigInt& mod) const {
    BigInt base = *this % mod;
    BigInt result = 1;
    BigInt exponent = exp;

    while (exponent > 0) {
        if (exponent % 2 == 1) {
            result = result.modMul(base, mod);
        }
        base = base.modMul(base, mod);
        exponent = exponent / 2;
    }
    return result;
}

