#include "BigUint.h"
#include <stdexcept>
#include <complex>
#include <cmath>
#include <execution>
#include <vector>
#include <numbers>
#include <ranges>


const BigUint BigUint::ZERO = BigUint();
const BigUint BigUint::ONE = BigUint(static_cast<DigitType>(1));
const BigUint BigUint::TWO = BigUint(static_cast<DigitType>(2));
const BigUint BigUint::TEN = BigUint(static_cast<DigitType>(10));

BigUint::BigUint()
    : digits_(1, 0)
{}

BigUint::BigUint(DigitType value)
    : digits_(1, value)
{}

BigUint::BigUint(const std::string& str) {
    fromString(str);
}

BigUint::BigUint(const std::vector<DigitType> &digits)
    : digits_(digits) {
    if (digits_.empty()) {
        digits_.push_back(static_cast<DigitType>(0));
    }

    std::ranges::reverse(digits_.begin(), digits_.end());
}

void BigUint::setDigits(const Digits &digits) {
    digits_ = digits;
    removeLeadingZeros();
    if (digits_.empty()) {
        *this = BigUint::ZERO;
    };
}

std::string BigUint::toString() const {
    if (*this == ZERO) return "0";
    if (*this == ONE) return "1";
    if (*this == TWO) return "2";

    std::string result;
    result += std::to_string(digits_.back());
    for (int ii = static_cast<int>(digits_.size()) - 2; ii >= 0; ii--) {
        result += '|';
        result += std::to_string(digits_[ii]);
    }

    return result;
}

BigUint BigUint::fromString(const std::string &str) {
    if (str.empty()) {
        *this = ZERO;
        return *this;
    };

    digits_.clear();
    std::stringstream ss(str);
    std::string segment;

    // ✅ Split the string by '|'
    while (std::getline(ss, segment, '|')) {
        try {
            const auto value = static_cast<DigitType>(std::stoul(segment));  // ✅ Convert string to uint16_t
            if (value >= BASE) throw std::out_of_range("Digit value exceeds BASE.");
            digits_.push_back(value);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid digit in BigUint string.");
        }
    }

    // ✅ Store in reverse order (LSB first)
    std::reverse(digits_.begin(), digits_.end());
    removeLeadingZeros();
    return *this;
}

std::ostream& operator<<(std::ostream& os, const BigUint& bigUint) {
    os << bigUint.toString();
    return os;
}

std::istream& operator>>(std::istream& is, BigUint& bigUint) {
    std::string value;
    is >> value;
    bigUint = BigUint(value);
    return is;
}

std::strong_ordering BigUint::operator<=>(const BigUint& other) const {
    if (digits_.size() < other.digits_.size()) return std::strong_ordering::less;
    if (digits_.size() > other.digits_.size()) return std::strong_ordering::greater;

    for (std::ptrdiff_t ii = static_cast<std::ptrdiff_t>(digits_.size()) - 1; ii >= 0; --ii) {
        if (digits_[ii] < other.digits_[ii]) return std::strong_ordering::less;
        if (digits_[ii] > other.digits_[ii]) return std::strong_ordering::greater;
    }

    return std::strong_ordering::equal;
}

BigUint BigUint::shiftMeLeft(const size_t shiftPositions) {
    if (shiftPositions == 0) {
        return *this;
    }

    if (*this == BigUint::ZERO) {
        return *this;
    }

    const auto previousSize = digits_.size();
    const auto newSize = previousSize + shiftPositions;
    digits_.resize(newSize);
    for (std::size_t ii = newSize - 1; ii >= shiftPositions; ii--) {
        digits_[ii] = digits_[ii - shiftPositions];
    }

    for (std::size_t ii = 0; ii < shiftPositions; ii++) {
        digits_[ii] = 0;
    }

    return *this;
}

BigUint BigUint::shiftLeft(const size_t shiftPositions) const {
    BigUint result = *this;
    return result.shiftMeLeft(shiftPositions);
}

BigUint BigUint::mePlusOne() {
    if (*this == ZERO) {
        *this = ONE;
        return *this;
    }

    if (*this == ONE) {
        *this = TWO;
        return *this;
    }

    std::size_t currentDigitPosition = 0;
    while (currentDigitPosition < digits_.size()) {
        auto &currentDigit = digits_[currentDigitPosition];
        if (currentDigit < BASE - 1) {
            ++currentDigit;
            return *this;
        }

        currentDigit = 0;
        ++currentDigitPosition;
    }

    digits_.push_back(1);
    return *this;
}

BigUint BigUint::plusOne() const {
    auto result = *this;
    return result.mePlusOne();
}

BigUint BigUint::meMinusOne() {
    if (*this == ZERO) {
        throw std::runtime_error("Invalid negative result expected!");
    }

    if (*this == ONE) {
        *this = ZERO;
        return *this;
    }

    if (*this == TWO) {
        *this = ONE;
        return *this;
    }

    std::size_t currentDigitPosition = 0;
    while (currentDigitPosition < digits_.size()) {
        auto &currentDigit = digits_[currentDigitPosition];
        if (currentDigit > 0) {
            --currentDigit;
            break;
        }

        currentDigit = BASE - 1;
        currentDigitPosition++;
    }

    if (digits_.back() == 0) {
        digits_.pop_back();
    }

    return *this;
}

BigUint BigUint::minusOne() const {
    auto result = *this;
    return result.meMinusOne();
}

BigUint BigUint::addToMe(DigitType digit) {
    if (digit == 0) {
        return *this;
    }

    if (*this == BigUint::ZERO) {
        digits_[0] = digit;
        return *this;
    }

    if (digit == 1) {
        return mePlusOne();
    }

    DigitType carry = digit;
    for (auto &currentDigit : digits_) {
        WideDigitType partialSum = currentDigit + carry;
        if (partialSum < BigUint::BASE) {
            currentDigit =static_cast<DigitType>(partialSum);
            carry = 0;
            break;
        }

        currentDigit = static_cast<DigitType>(partialSum - BigUint::BASE);
        carry = 1;
    }

    if (carry) {
        digits_.push_back(1);
    }

    return *this;
}

BigUint BigUint::add(DigitType digit) const {
    BigUint result = *this;
    result.addToMe(digit);
    return result;
}

BigUint BigUint::operator+=(DigitType digit) {
    return addToMe(digit);
}

BigUint BigUint::operator+(DigitType digit) const {
    return add(digit);
}

BigUint BigUint::addToMe(const BigUint &rhs) {
    // To avoid this == &rhs case.
    BigUint other = rhs;

    if (*this == BigUint::ZERO) {
        *this = other;
        return other;
    }

    if (other == BigUint::ZERO) {
        return *this;
    }

    std::vector<DigitType> result;
    const auto minSize = std::min(digits_.size(), other.digits_.size());
    DigitType carry = 0;
    for (std::size_t ii = 0; ii < minSize; ++ii) {
        WideDigitType sum = carry;
        sum += digits_[ii];
        sum += other.digits_[ii];

        DigitType currentDigit;
        if (sum >= BigUint::BASE) {
            currentDigit = static_cast<DigitType>(sum - BigUint::BASE);
            carry = 1;
        }
        else {
            currentDigit = static_cast<DigitType>(sum);
            carry = 0;
        }
        result.push_back(currentDigit);
    }

    for (std::size_t ii = minSize; ii < digits_.size(); ii++) {
        const WideDigitType sum = digits_[ii] + carry;
        DigitType currentDigit;
        if (sum >= BigUint::BASE) {
            currentDigit = static_cast<DigitType>(sum - BigUint::BASE);
            carry = 1;
        }
        else {
            currentDigit = static_cast<DigitType>(sum);
            carry = 0;
        }
        result.push_back(currentDigit);
    }

    for (std::size_t ii = minSize; ii < other.digits_.size(); ii++) {
        const WideDigitType sum = other.digits_[ii] + carry;
        DigitType currentDigit;
        if (sum >= BigUint::BASE) {
            currentDigit = static_cast<DigitType>(sum - BigUint::BASE);
            carry = 1;
        }
        else {
            currentDigit = static_cast<DigitType>(sum);
            carry = 0;
        }
        result.push_back(currentDigit);
    }

    if (carry == 1) {
        result.push_back(1);
    }

    digits_ = result;
    return *this;
}

BigUint BigUint::add(const BigUint &rhs) const {
    auto result = *this;
    return result.addToMe(rhs);
}

BigUint BigUint::operator+=(const BigUint &rhs) {
    return addToMe(rhs);
}

BigUint BigUint::operator+(const BigUint &rhs) const {
    return add(rhs);
}

BigUint BigUint::subtractToMe(const BigUint &rhs) {
    // To avoid this == &rhs case.
    BigUint other = rhs;

    if (*this < other) {
        throw std::invalid_argument("Invalid negative result expected!");
    }

    if (*this == other) {
        return BigUint::ZERO;
    }

    std::vector<DigitType> result;
    result.reserve(digits_.size());
    DigitType carry = 0;
    std::size_t currentDigitPosition = 0;
    for (; currentDigitPosition < other.digits_.size(); currentDigitPosition++) {
        WideDigitType currentDigit = digits_[currentDigitPosition];
        const WideDigitType otherCurrentDigit = other.digits_[currentDigitPosition] + carry;
        if (currentDigit > otherCurrentDigit) {
            result.push_back(static_cast<DigitType>(currentDigit - otherCurrentDigit));
            carry = 0;
        } else {
            currentDigit += BigUint::BASE;
            result.push_back((static_cast<DigitType>(currentDigit - otherCurrentDigit)));
            carry = 1;
        }
    }

    while (carry == 1 && currentDigitPosition < digits_.size()) {
        DigitType currentDigit = digits_[currentDigitPosition];
        if (currentDigit > 0) {
            result.push_back(currentDigit - 1);
            carry = 0;
        } else {
            result.push_back(BigUint::BASE - 1);
            // carry = 1;
        }
        currentDigitPosition++;
    }

    for (; currentDigitPosition < digits_.size(); currentDigitPosition++) {
        result.push_back(digits_[currentDigitPosition]);
    }

    digits_ = result;;
    removeLeadingZeros();
    return *this;
}

BigUint BigUint::subtract(const BigUint &rhs) const {
    auto other = rhs;
    return other.subtractToMe(other);
}

BigUint BigUint::operator-=(const BigUint &rhs) {
    return subtractToMe(rhs);
}

BigUint  BigUint::operator-(const BigUint &rhs) const {
    return subtract(rhs);
}

BigUint BigUint::multiplyMeByOneDigit(DigitType digit) {
    if (digit == 0) {
        *this = BigUint::ZERO;
        return *this;
    }

    if (digit == 1) {
        return *this;
    }

    if (*this == BigUint::ZERO) {
        return *this;
    }

    if (*this == BigUint::ONE) {
        digits_[0] = digit;
        return *this;
    }

    DigitType carry = 0;
    for (auto &thisDigit: digits_) {
        const WideDigitType product = thisDigit * digit + carry;
        DigitType calculatedDigit = 0;
        if (product >= BASE) {
            WideDigitType aux = product / BASE;
            carry = static_cast<DigitType>(aux);
            aux = product - aux * BASE;
            calculatedDigit = static_cast<DigitType>(aux);
        }
        else {
            carry = 0;
            calculatedDigit = static_cast<DigitType>(product);
        }
        thisDigit = calculatedDigit;
    }

    if (carry != 0) {
        digits_.push_back(carry);
    }

    return *this;
}

BigUint BigUint::multiplyByOneDigit(const DigitType digit) const {
    BigUint result = *this;
    return result.multiplyMeByOneDigit(digit);
}

BigUint BigUint::operator*=(const DigitType digit) {
    return multiplyByOneDigit(digit);
}

BigUint BigUint::operator*(const DigitType digit) const {
    return multiplyByOneDigit(digit);
}

BigUint BigUint::multiplyMeBy(const BigUint &rhs) {
    // To avoid this == &rhs case.
    BigUint other = rhs;
    return multiplyNaive(other);
}

BigUint BigUint::multiplyBy(const BigUint &rhs) const {
    BigUint result = *this;
    return result.multiplyMeBy(rhs);
}

BigUint BigUint::operator*=(const BigUint &rhs) {
    return multiplyMeBy(rhs);
}

BigUint BigUint::operator*(const BigUint &rhs) const {
    return multiplyBy(rhs);
}

// returns the remainder
BigUint::DigitType BigUint::divideMeByOneDigit(const DigitType divisor) {
    if (divisor == 0) {
        throw std::runtime_error("division by zero");
    }

    if (divisor == 1) {
        return 0;
    }

    if (*this == BigUint::ZERO) {
        return 0;
    }

    if (digits_.size() == 1) {
        const DigitType remainder = digits_.front() % divisor;
        digits_.front() /= divisor;
        return remainder;
    }

    int digitPosition = static_cast<int>(digits_.size()) - 1;
    const DigitType &firstDigit = digits_[digitPosition];
    BigUint::WideDigitType carry = 0;
    std::size_t numberOfQuotients = digits_.size();
    if (firstDigit < divisor) {
        carry = firstDigit;
        numberOfQuotients--;
        digitPosition--;
    }

    std::vector<DigitType> quotients;
    while (digitPosition >= 0) {
        const DigitType &currentDigit = digits_[digitPosition];
        const WideDigitType dividend = carry * BigUint::BASE + currentDigit;
        const auto remainder = static_cast<DigitType>(dividend % divisor);
        const auto quotient = static_cast<DigitType> (dividend / divisor);
        quotients.push_back(quotient);
        carry = remainder;
        digitPosition--;
    }

    digits_ = quotients;
    std::ranges::reverse(digits_.begin(), digits_.end());
    return static_cast<BigUint::DigitType>(carry);
}

// returns quotient and remainder
std::pair<BigUint, BigUint::DigitType> BigUint::divideByOneDigit(const DigitType divisor) const {
    BigUint quotient = *this;
    auto remainder = quotient.divideMeByOneDigit(divisor);
    return {quotient, remainder};
}

// returns the quotient
BigUint BigUint::operator/=(DigitType digit) {
    divideMeByOneDigit(digit);
    return *this;
}

// returns the quotient
BigUint BigUint::operator/(DigitType digit) const {
    const auto [quotient, remainder] = divideByOneDigit(digit);
    return quotient;
}

// returns the remainder
BigUint::DigitType BigUint::operator%=(DigitType digit) {
    return divideMeByOneDigit(digit);
}

// returns the remainder
BigUint::DigitType BigUint::operator%(DigitType digit) const {
    const auto [quotient, remainder] = divideByOneDigit(digit);
    return remainder;
}

// returns the remainder
BigUint BigUint::divideMeBy(const BigUint &rhs) {
    // &rhs == this is already managed in divide
    const auto [quotient, remainder] = divide(*this, rhs);
    *this = quotient;
    return remainder;
}

// returns quotient and remainder
[[nodiscard]] std::pair<BigUint, BigUint> BigUint::divideBy(const BigUint &rhs) const {
    return divide(*this, rhs);
}

// returns the quotient
BigUint BigUint::operator/=(const BigUint &rhs) {
    // &rhs == this is already managed in divide
    const auto [quotient, _] = divide(*this, rhs);
    *this = quotient;
    return quotient;
}

// returns the quotient
BigUint BigUint::operator/(const BigUint &rhs) const {
    const auto [quotient, _] = divideBy(rhs);
    return quotient;
}

// returns the remainder
BigUint BigUint::operator%=(const BigUint &rhs) {
    return divideMeBy(rhs);
}

// returns the remainder
[[nodiscard]] BigUint BigUint::operator%(const BigUint &rhs) const {
    const auto [_, remainder] = divideBy(rhs);
    return remainder;
}

std::string BigUint::toBase10String() const {
    if (*this == ZERO) return "0";
    if (*this == ONE) return "1";
    if (*this == TWO) return "2";

    if (digits_.size() == 1) {
        return std::to_string(digits_[0]);
    }

    BigUint value = *this;
    std::string result;
    while (value.digits_.size() > 1) {
        const auto [quotient, remainder] = value.divideByOneDigit(10);
        result.push_back('0' + static_cast<char>(remainder));
        value = quotient;
    }

    std::ranges::reverse(result.begin(), result.end());
    return std::to_string(value.digits_[0]) + result;
}

void BigUint::fromBase10String(const std::string& str) {
    if (str.empty()) throw std::invalid_argument("Empty string is not a valid number.");

    *this = BigUint::ZERO;
    for (auto d : str) {
        if (!std::isdigit(d)) throw std::invalid_argument("Building BigUint: Invalid character");
        *this = (*this * BigUint::TEN) + BigUint(static_cast<Digit>(d) - static_cast<Digit>('0'));
    }
}

/*
BigUint BigUint::modAdd(const BigUint& other, const BigUint& mod) const {
    BigUint result = addMe(other) % mod;
    return result;
}

BigUint BigUint::modSub(const BigUint& other, const BigUint& mod) const {
    BigUint result = (*this - other) % mod;
    return result;
}

BigUint BigUint::modMul(const BigUint& other, const BigUint& mod) const {
    BigUint result = (*this * other) % mod;
    return result;
}

BigUint BigUint::modPow(const BigUint& exponent, const BigUint& mod) const {
    if (mod == BigUint::ZERO) {
        throw std::invalid_argument("Modulo by zero is not allowed");
    }

    BigUint base = *this % mod;  // Reduce base modulo mod
    BigUint exp = exponent;
    BigUint result = BigUint::ONE;

    while (exp > BigUint::ZERO) {
        if (exp.digits_[0] % 2 == 1) {  // If exp is odd, multiply result by base
            result = (result * base) % mod;
        }
        base = (base * base) % mod;  // Square the base
        exp = exp / BigUint(2);  // Divide exponent by 2
    }

    return result;
}
*/

void BigUint::removeLeadingZeros() {
    while (digits_.size() > 1 && digits_.back() == 0) {
        digits_.pop_back();
    }

    if (digits_.empty()) {
        digits_.push_back(0);
    }
}

BigUint BigUint::multiplyNaive(const BigUint& other) const {
    if (other.digits_.size() == 1) {
        return multiplyByOneDigit(other.digits_.front());
    }

    if (digits_.size() == 1) {
        return other.multiplyByOneDigit(digits_.front());
    }

    std::vector<BigUint> partialMultiplications(other.digits_.size());
    std::size_t shiftCounter = 0;
    for (const auto otherCurrentDigit : other.digits_) {
        BigUint currentMultiplication = this->multiplyByOneDigit(otherCurrentDigit);
        currentMultiplication.shiftMeLeft(shiftCounter);
        partialMultiplications[shiftCounter] = currentMultiplication;
        shiftCounter++;
    }

    BigUint result;
    for (const auto &multiplication : partialMultiplications) {
        result += multiplication;
    }

    return result;
}

std::pair<BigUint, BigUint> BigUint::divide(const BigUint &dividend, const BigUint &divisor) {
    if (divisor == BigUint::ZERO) {
        throw std::invalid_argument("Division by zero is not allowed.");
    }

    if (divisor == BigUint::ONE) {
        return {dividend, BigUint::ZERO};
    }

    if (dividend == divisor) {
        return {BigUint::ONE, BigUint::ZERO};
    }

    if (dividend < divisor) {
        return {BigUint::ZERO, dividend};
    }

    if (divisor.digits_.size() == 1) {
        const auto digit = divisor.digits_.front();
        const auto [quotient, remainder] = dividend.divideByOneDigit(digit);
        const BigUint remainderAsUint(remainder);
        return {quotient, remainderAsUint};
    }

    BigUint quotient, remainder;
    remainder.digits_.resize(dividend.digits_.size());

    // ✅ Process digits from most to least significant
    for (int i = static_cast<int>(dividend.digits_.size()) - 1; i >= 0; --i) {
        remainder.digits_.insert(remainder.digits_.begin(), dividend.digits_[i]);  // Shift remainder
        remainder.removeLeadingZeros();

        DigitType q = 0;
        if (remainder >= divisor) {
            // ✅ Use binary search to find the largest `q` such that `q * divisor <= remainder`
            WideDigitType low = 0, high = BASE - 1;
            while (low <= high) {
                const WideDigitType lowPlusHigh = (low + high);
                const auto mid = static_cast<DigitType>(lowPlusHigh / 2);
                const BigUint test = divisor * BigUint(mid);

                if (test <= remainder) {
                    q = mid;
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
            }

            remainder = remainder - (divisor * BigUint(q));
        }

        quotient.digits_.insert(quotient.digits_.begin(), q);
    }

    quotient.removeLeadingZeros();
    remainder.removeLeadingZeros();
    return {quotient, remainder};
}

/*
using Complex = std::complex<double>;
const double PI = acos(-1);

void fft(std::vector<std::complex<double>>& a, bool invert) {
    size_t n = a.size();
    if (n == 1) return;

    const double angle = 2 * std::numbers::pi / n * (invert ? -1 : 1);
    const std::complex<double> wn(cos(angle), sin(angle));
    std::complex<double> w(1);

    std::vector<std::complex<double>> a0(n / 2), a1(n / 2);
    for (size_t i = 0; i < n / 2; ++i) {
        a0[i] = a[i * 2];
        a1[i] = a[i * 2 + 1];
    }

    fft(a0, invert);
    fft(a1, invert);

    for (size_t i = 0; i < n / 2; ++i) {
        std::complex<double> temp = w * a1[i];
        a[i] = a0[i] + temp;
        a[i + n / 2] = a0[i] - temp;
        if (invert) {
            a[i] /= 2;
            a[i + n / 2] /= 2;
        }
        w *= wn;
    }
}

BigUint BigUint::multiplyFFT(const BigUint& b) const {
    // Convert BigUint digits to complex vectors
    std::vector<std::complex<double>> fa(this->digits_.begin(), this->digits_.end());
    std::vector<std::complex<double>> fb(b.digits_.begin(), b.digits_.end());

    // Find the next power of 2 greater than size of both numbers
    size_t n = 1;
    while (n < this->digits_.size() + b.digits_.size()) n <<= 1;
    fa.resize(n);
    fb.resize(n);

    // Perform FFT
    fft(fa, false);
    fft(fb, false);

    // Point-wise multiplication
    for (size_t i = 0; i < n; ++i) {
        fa[i] *= fb[i];
    }

    // Inverse FFT
    fft(fa, true);

    // Convert the result back to BigUint
    std::vector<BigUint::DigitType> result(n);
    WideDigitType carry = 0;
    for (size_t ii = 0; ii < n; ++ii) {
        WideDigitType sum = static_cast<WideDigitType>(round(fa[ii].real())) + carry;
        result[ii] = static_cast<DigitType>(sum % BigUint::BASE);
        carry = sum / BigUint::BASE;
    }

    // Remove leading zeros
    while (result.size() > 1 && result.back() == 0) {
        result.pop_back();
    }

    BigUint res;
    res.digits_ = result;
    return res;
}

std::pair<BigUint, BigUint> BigUint::split(size_t pos) const {
    if (pos >= digits_.size()) {
        return {BigUint(0), *this};  // If split position is too large, return (0, full number)
    }

    BigUint low, high;
    low.digits_.assign(digits_.begin(), digits_.begin() + pos);
    high.digits_.assign(digits_.begin() + pos, digits_.end());

    // Remove leading zeros in the high part
    high.removeLeadingZeros();

    return {low, high};
}

BigUint BigUint::multiplyKaratsuba(const BigUint& other) const {
    if (digits_.size() < 32 || other.digits_.size() < 32) {
        return multiplyNaive(other); // Use naïve for small numbers
    }

    size_t m = digits_.size() / 2;

    // Split numbers into high and low parts
    auto [low1, high1] = split(m);
    auto [low2, high2] = other.split(m);

    BigUint z0 = low1.multiplyKaratsuba(low2);
    BigUint z1 = (low1 + high1).multiplyKaratsuba(low2 + high2);
    BigUint z2 = high1.multiplyKaratsuba(high2);

    return (z2.shiftLeft(2 * m) + (z1 - z2 - z0).shiftLeft(m) + z0);
}
*/

/*
BigUint BigUint::operator+(const BigUint& other) const {
    if (*this == BigUint::ZERO) {
        return other;
    }

    if (other == BigUint::ZERO) {
        return *this;
    }

    std::vector<DigitType> result;
    const auto minSize = std::min(digits_.size(), other.digits_.size());
    DigitType carry = 0;
    for (std::size_t ii = 0; ii < minSize || carry; ++ii) {
        WideDigitType sum = carry;
        sum += digits_[ii];
        sum += other.digits_[ii];

        DigitType currentDigit;
        if (sum >= BigUint::BASE) {
            currentDigit = static_cast<DigitType>(sum - BigUint::BASE);
            carry = 1;
        }
        else {
            currentDigit = static_cast<DigitType>(sum);
            carry = 0;
        }
        result.push_back(currentDigit);
    }

    for (std::size_t ii = minSize; ii < digits_.size(); ii++) {
        const WideDigitType sum = digits_[ii] + carry;
        DigitType currentDigit;
        if (sum >= BigUint::BASE) {
            currentDigit = static_cast<DigitType>(sum - BigUint::BASE);
            carry = 1;
        }
        else {
            currentDigit = static_cast<DigitType>(sum);
            carry = 0;
        }
        result.push_back(currentDigit);
    }

    for (std::size_t ii = minSize; ii < other.digits_.size(); ii++) {
        const WideDigitType sum = other.digits_[ii] + carry;
        DigitType currentDigit;
        if (sum >= BigUint::BASE) {
            currentDigit = static_cast<DigitType>(sum - BigUint::BASE);
            carry = 1;
        }
        else {
            currentDigit = static_cast<DigitType>(sum);
            carry = 0;
        }
        result.push_back(currentDigit);
    }

    if (carry == 1) {
        result.push_back(1);
    }

    std::reverse(result.begin(), result.end());
    return BigUint(result);
}

BigUint BigUint::operator-(const BigUint& other) const {
    if (*this < other) {
        throw std::invalid_argument("Invalid negative result expected!");
    }

    if (*this == other) {
        return BigUint::ZERO;
    }

    std::vector<DigitType> result;
    result.reserve(digits_.size());
    DigitType carry = 0;
    std::size_t currentDigitPosition = 0;
    for (; currentDigitPosition < other.digits_.size(); currentDigitPosition++) {
        WideDigitType currentDigit = digits_[currentDigitPosition];
        const WideDigitType otherCurrentDigit = other.digits_[currentDigitPosition] + carry;
        if (currentDigit > otherCurrentDigit) {
            result.push_back(static_cast<DigitType>(currentDigit - otherCurrentDigit));
            carry = 0;
        } else {
            currentDigit += BigUint::BASE;
            result.push_back((static_cast<DigitType>(currentDigit - otherCurrentDigit)));
            carry = 1;
        }
    }

    while (carry == 1 && currentDigitPosition < digits_.size()) {
        DigitType currentDigit = digits_[currentDigitPosition];
        if (currentDigit > 0) {
            result.push_back(currentDigit - 1);
            carry = 0;
        } else {
            result.push_back(BigUint::BASE - 1);
            // carry = 1;
        }
        currentDigitPosition++;
    }

    for (; currentDigitPosition < digits_.size(); currentDigitPosition++) {
        result.push_back(digits_[currentDigitPosition]);
    }

    BigUnt remainder(result);
    remainder.removeLeadingZeros();
    return remainder;
}

BigUint BigUint::operator*(const BigUint& other) const {
    return this->multiplyNaive(other);
    // return this->multiplyKaratsuba(other);
    // return this->multiplyFFT(other);
}

BigUint BigUint::operator/(const BigUint& divisor) const {
    if (divisor == BigUint::ZERO) {
        throw std::invalid_argument("Division by zero is not allowed!");
    }

    // ✅ Special Case 1: Dividing by 1
    if (divisor == BigUint::ONE) return *this;

    // ✅ Special Case 2: If divisor > dividend, result is always 0
    if (*this < divisor) return BigUint::ZERO;

     // ✅ Special Case 3: If divisor == dividend, result is always 1
     if (*this == divisor) return BigUint::ONE;

    BigUint dividend = *this;
    BigUint quotient;
    quotient.digits_.resize(dividend.digits_.size(), 0);

    BigUint current;
    for (int i = static_cast<int>(dividend.digits_.size()) - 1; i >= 0; --i) {
        current.digits_.insert(current.digits_.begin(), dividend.digits_[i]);
        current.removeLeadingZeros();

        DigitType count = 0;
        while (current >= divisor) {
            current = current - divisor;
            ++count;
        }
        quotient.digits_[i] = count;
    }

    quotient.removeLeadingZeros();
    return quotient;
}

BigUint BigUint::operator%(const BigUint& other) const {
    if (other == BigUint::ZERO) {
        throw std::invalid_argument("Modulus by zero");
    }

    // ✅ Special Case 1: If *this < other, result is always *this
    if (*this < other) return *this;

    //✅ Special Case 2: If *this == other, result is always zero
    if (*this == other) return BigUint::ZERO;

    const BigUint quotient = (*this / other);
    const BigUint product = quotient * other;

    // BigUint remainder = *this - (*this / other) * other;
    BigUint remainder = *this - product;

    return remainder;
}

BigUint& BigUint::operator+=(const BigUint& other) {
    *this = *this + other;
    return *this;
}

BigUint& BigUint::operator-=(const BigUint& other) {
    *this = *this - other;
    return *this;
}

BigUint& BigUint::operator*=(const BigUint& other) {
    *this = *this * other;
    return *this;
}

BigUint& BigUint::operator/=(const BigUint& other) {
    *this = *this / other;
    return *this;
}

BigUint& BigUint::operator%=(const BigUint& other) {
    *this = *this % other;
    return *this;
}
*/