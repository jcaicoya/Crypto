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

    while (std::getline(ss, segment, '|')) {
        try {
            const auto value = static_cast<DigitType>(std::stoul(segment));  // ✅ Convert string to uint16_t
            if (value >= BASE) throw std::out_of_range("Digit value exceeds BASE.");
            digits_.push_back(value);
        } catch (const std::exception&) {
            throw std::invalid_argument("Invalid digit in BigUint string.");
        }
    }

    std::ranges::reverse(digits_.begin(), digits_.end());
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

void BigUint::shiftMeLeft(const size_t shiftPositions) {
    if (shiftPositions == 0) {
        return;
    }

    if (*this == BigUint::ZERO) {
        return;
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
}

BigUint BigUint::shiftLeft(const size_t shiftPositions) const {
    BigUint result = *this;
    result.shiftMeLeft(shiftPositions);
    return result;
}

void BigUint::mePlusOne() {
    if (*this == ZERO) {
        *this = ONE;
        return;
    }

    if (*this == ONE) {
        *this = TWO;
        return;
    }

    std::size_t currentDigitPosition = 0;
    while (currentDigitPosition < digits_.size()) {
        auto &currentDigit = digits_[currentDigitPosition];
        if (currentDigit < BASE - 1) {
            ++currentDigit;
            return;
        }

        currentDigit = 0;
        ++currentDigitPosition;
    }

    digits_.push_back(1);
}

BigUint BigUint::plusOne() const {
    auto result = *this;
    result.mePlusOne();
    return result;
}

void BigUint::meMinusOne() {
    if (*this == ZERO) {
        throw std::runtime_error("Invalid negative result expected!");
    }

    if (*this == ONE) {
        *this = ZERO;
        return;
    }

    if (*this == TWO) {
        *this = ONE;
        return;
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
}

BigUint BigUint::minusOne() const {
    auto result = *this;
    result.meMinusOne();
    return result;
}

void BigUint::addToMe(DigitType digit) {
    if (digit == 0) {
        return;
    }

    if (*this == BigUint::ZERO) {
        digits_[0] = digit;
        return;
    }

    if (digit == 1) {
        mePlusOne();
        return;
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
}

BigUint BigUint::add(DigitType digit) const {
    BigUint result = *this;
    result.addToMe(digit);
    return result;
}

void BigUint::operator+=(DigitType digit) {
    addToMe(digit);
}

BigUint BigUint::operator+(DigitType digit) const {
    return add(digit);
}

void BigUint::addToMe(const BigUint &rhs) {
    if (rhs == BigUint::ZERO) {
        return;
    }

    if (*this == BigUint::ZERO) {
        *this = rhs;
        return;
    }

    std::vector<DigitType> result;
    const auto minSize = std::min(digits_.size(), rhs.digits_.size());
    DigitType carry = 0;
    for (std::size_t ii = 0; ii < minSize; ++ii) {
        WideDigitType sum = carry;
        sum += digits_[ii];
        sum += rhs.digits_[ii];

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

    for (std::size_t ii = minSize; ii < rhs.digits_.size(); ii++) {
        const WideDigitType sum = rhs.digits_[ii] + carry;
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
}

BigUint BigUint::add(const BigUint &rhs) const {
    auto result = *this;
    result.addToMe(rhs);
    return result;
}

void BigUint::operator+=(const BigUint &rhs) {
    addToMe(rhs);
}

BigUint BigUint::operator+(const BigUint &rhs) const {
    return add(rhs);
}

void BigUint::subtractToMe(const BigUint &rhs) {
    if (*this < rhs) {
        throw std::invalid_argument("Invalid negative result expected!");
    }

    if (rhs == BigUint::ZERO) {
        return;
    }

    if (*this == rhs) {
        *this = BigUint::ZERO;
        return;
    }

    std::vector<DigitType> result;
    result.reserve(digits_.size());
    DigitType carry = 0;
    std::size_t currentDigitPosition = 0;
    for (; currentDigitPosition < rhs.digits_.size(); currentDigitPosition++) {
        WideDigit currentDigit = digits_[currentDigitPosition];
        if (const WideDigit otherCurrentDigit = rhs.digits_[currentDigitPosition] + carry; currentDigit > otherCurrentDigit) {
            result.push_back(static_cast<DigitType>(currentDigit - otherCurrentDigit));
            carry = 0;
        } else {
            currentDigit += BigUint::BASE;
            result.push_back((static_cast<DigitType>(currentDigit - otherCurrentDigit)));
            carry = 1;
        }
    }

    while (carry == 1 && currentDigitPosition < digits_.size()) {
        if (const Digit currentDigit = digits_[currentDigitPosition]; currentDigit > 0) {
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
}

BigUint BigUint::subtract(const BigUint &rhs) const {
    auto result = *this;
    result.subtractToMe(rhs);
    return result;
}

void BigUint::operator-=(const BigUint &rhs) {
    return subtractToMe(rhs);
}

BigUint  BigUint::operator-(const BigUint &rhs) const {
    return subtract(rhs);
}

void BigUint::multiplyMeByOneDigit(DigitType digit) {
    if (digit == 0) {
        *this = BigUint::ZERO;
        return;
    }

    if (digit == 1) {
        return;
    }

    if (*this == BigUint::ZERO) {
        return;
    }

    if (*this == BigUint::ONE) {
        digits_[0] = digit;
        return;
    }

    DigitType carry = 0;
    for (auto &thisDigit: digits_) {
        const WideDigitType product = thisDigit * digit + carry;
        DigitType calculatedDigit = 0;
        if (product >= BASE) {
            WideDigitType aux = product / BASE;
            carry = static_cast<DigitType>(aux);
            aux = product - carry * BASE;
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
}

BigUint BigUint::multiplyByOneDigit(const DigitType digit) const {
    BigUint result = *this;
    result.multiplyMeByOneDigit(digit);
    return result;
}

void BigUint::operator*=(const DigitType digit) {
    multiplyMeByOneDigit(digit);
}

BigUint BigUint::operator*(const DigitType digit) const {
    return multiplyByOneDigit(digit);
}

void BigUint::multiplyMeBy(const BigUint &rhs) {
    *this = multiplyNaive(rhs);
}

BigUint BigUint::multiplyBy(const BigUint &rhs) const {
    BigUint result = *this;
    result.multiplyMeBy(rhs);
    return result;
}

void BigUint::operator*=(const BigUint &rhs) {
    multiplyMeBy(rhs);
}

BigUint BigUint::operator*(const BigUint &rhs) const {
    return multiplyBy(rhs);
}

void BigUint::squareMe() {
    *this = square();
}

[[nodiscard]] BigUint BigUint::square() const {
    if (*this == BigUint::ZERO) {
        return BigUint::ZERO;
    }

    if (*this == BigUint::ONE) {
        return BigUint::ONE;
    }

    if (digits_.size() == 1) {
        const auto digit = static_cast<WideDigit>(digits_[0]);
        const auto square = digit * digit;
        if (square < BigUint::BASE) {
            return BigUint(static_cast<Digit>(square));
        }

        const auto quotient = square / BigUint::BASE;
        const auto remainder = square % BigUint::BASE;
        BigUint result;
        result.digits_.resize(2);
        result.digits_[0] = static_cast<Digit>(remainder);
        result.digits_[1] = static_cast<Digit>(quotient);
        return result;
    }

    const auto n = digits_.size();
    std::vector<std::vector<WideDigitType>> matrix(n, std::vector<WideDigitType>(n, static_cast<WideDigit>(0)));
    for (std::size_t ii = 0; ii < n; ii++) {
        for (std::size_t jj = ii; jj < n; jj++) {
            matrix[ii][jj] = static_cast<WideDigit>(digits_[ii] * digits_[jj]);
        }
    }

    for (std::size_t jj = 0; jj < n; jj++) {
        for (std::size_t ii = jj + 1; ii < n; ii++) {
            matrix[ii][jj] = matrix[jj][ii];
        }
    }

    for (std::size_t ii = 0; ii < n; ii++) {
        WideDigitType carry = 0;
        for (std::size_t jj = 0; jj < n; jj++) {
            const auto current = matrix[ii][jj] + carry;
            if (current >= BigUint::BASE) {
                carry = current / BigUint::BASE;
                matrix[ii][jj] = current - carry * BigUint::BASE;
            }
            else {
                matrix[ii][jj] = current;
                carry = 0;
            }
        }
        if (carry > 0) {
            matrix[ii].push_back(carry);
        }
    }

    for (std::size_t ii = 1; ii < n; ii++) {
        const auto previousSize = matrix[ii].size();
        const auto newSize = previousSize + ii;
        matrix[ii].resize(newSize);
        for (std::size_t jj = newSize - 1; jj >= ii; jj--) {
            matrix[ii][jj] = matrix[ii][jj - 1];
        }
        for (std::size_t jj = ii; jj > 0; jj--) {
            matrix[ii][jj - 1] = static_cast<WideDigit>(0);
        }
    }

    const std::size_t resultsSize = matrix.back().size() + 1;
    std::vector<WideDigit> resultWideDigits(resultsSize);
    for (std::size_t jj = 0; jj < matrix.front().size(); jj++) {
        resultWideDigits[jj] = matrix.front()[jj];
    }

    for (std::size_t ii = 1; ii < matrix.size(); ii++) {
        const auto &currentRow = matrix[ii];
        WideDigit carry = 0;
        for (std::size_t jj = 0; jj < currentRow.size(); jj++) {
            const auto currentDigit = resultWideDigits[jj] + currentRow[jj] + carry;
            if (currentDigit >= BigUint::BASE) {
                carry = currentDigit / BigUint::BASE;
                resultWideDigits[jj] = currentDigit - carry * BigUint::BASE;
            }
            else {
                carry = 0;
                resultWideDigits[jj] = currentDigit;
            }
        }
        if (carry > 0) {
            resultWideDigits[currentRow.size()] = carry;
        }
    }

    if (resultWideDigits.back() == static_cast<WideDigit>(0)) {
        resultWideDigits.pop_back();
    }

    std::vector<DigitType> resultDigits(resultWideDigits.size());
    for (std::size_t jj = 0; jj < resultDigits.size(); jj++) {
        resultDigits[jj] = static_cast<DigitType>(resultWideDigits[jj]);
    }

    BigUint result;
    result.digits_ = resultDigits;
    return result;
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
    if (firstDigit < divisor) {
        carry = firstDigit;
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
        result += static_cast<char>('0' + remainder);
        value = quotient;
    }

    std::ranges::reverse(result.begin(), result.end());
    return std::to_string(value.digits_[0]) + result;
}

BigUint BigUint::fromBase10String(const std::string& str) {
    if (str.empty()) throw std::invalid_argument("Empty string is not a valid number.");

    BigUint result = BigUint::ZERO;
    for (auto d : str) {
        if (!std::isdigit(d)) throw std::invalid_argument("Building BigUint: Invalid character");
        result.multiplyMeByOneDigit(10);
        result.addToMe(BigUint(static_cast<Digit>(d) - static_cast<Digit>('0')));
    }

    return result;
}

BigUint BigUint::modAdd(const BigUint& lhs, const BigUint& rhs, const BigUint& mod) {
    if (mod == BigUint::ZERO) {
        throw std::runtime_error("modulus value cannot be zero");
    }

    if (mod == BigUint::ONE) {
        throw std::runtime_error("modulus value cannot be one");
    }

    const auto lhsMod = lhs % mod;
    const auto rhsMod = rhs % mod;
    auto result = lhsMod + rhsMod;
    if (result >= mod) {
        result -= mod;
    }

    return result;
}

BigUint BigUint::modSub(const BigUint& lhs, const BigUint& rhs, const BigUint& mod) {
    if (mod == BigUint::ZERO) {
        throw std::runtime_error("modulus value cannot be zero");
    }

    if (mod == BigUint::ONE) {
        throw std::runtime_error("modulus value cannot be one");
    }

    const auto lhsMod = lhs % mod;
    const auto rhsMod = rhs % mod;
    if (lhsMod >= rhsMod) {
        return lhsMod - rhsMod;
    }

    const auto temp = rhsMod - lhsMod;
    const auto result = mod - temp;
    return result;
}

BigUint BigUint::modMul(const BigUint& lhs, const BigUint& rhs, const BigUint& mod) {
    if (mod == BigUint::ZERO) {
        throw std::runtime_error("modulus value cannot be zero");
    }

    if (mod == BigUint::ONE) {
        throw std::runtime_error("modulus value cannot be one");
    }

    const auto lhsMod = lhs % mod;
    const auto rhsMod = rhs % mod;
    const auto result = lhsMod * rhsMod;
    return result % mod;
}

/*
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

    // Process digits from most to least significant
    for (int i = static_cast<int>(dividend.digits_.size()) - 1; i >= 0; --i) {
        remainder.digits_.insert(remainder.digits_.begin(), dividend.digits_[i]);  // Shift remainder
        remainder.removeLeadingZeros();

        DigitType q = 0;
        if (remainder >= divisor) {
            // Use binary search to find the largest `q` such that `q * divisor <= remainder`
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

using Complex = std::complex<double>;
const double PI = acos(-1);

// NOLINTNEXTLINE(misc-no-recursion)
void fft(std::vector<std::complex<double>>& a, bool invert) {
    size_t n = a.size();
    if (n == 1) return;

    const double angle = 2 * std::numbers::pi / static_cast<double>(n) * (invert ? -1 : 1);
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

    // Find the next power of 2 greater than the size of both numbers
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

std::pair<BigUint, BigUint> BigUint::split(std::size_t pos) const {
    if (pos >= digits_.size()) {
        return {BigUint(0), *this};
    }

    using diff_t = std::vector<int>::difference_type;
    BigUint low, high;
    low.digits_.assign(digits_.begin(), digits_.begin() + static_cast<diff_t>(pos));
    high.digits_.assign(digits_.begin() + static_cast<diff_t>(pos), digits_.end());
    high.removeLeadingZeros();
    return {low, high};
}

// NOLINTNEXTLINE(misc-no-recursion)
BigUint BigUint::multiplyKaratsuba(const BigUint& other) const {
    if (constexpr std::size_t minimumNumberOfDigits = 2; digits_.size() < minimumNumberOfDigits || other.digits_.size() < minimumNumberOfDigits) {
        return multiplyNaive(other);
    }

    const size_t middle = digits_.size() / 2;
    const auto [low1, high1] = split(middle);
    const auto [low2, high2] = other.split(middle);

    const BigUint z0 = low1.multiplyKaratsuba(low2);
    const BigUint z1 = (low1 + high1).multiplyKaratsuba(low2 + high2);
    const BigUint z2 = high1.multiplyKaratsuba(high2);

    return (z2.shiftLeft(2 * middle) + (z1 - z2 - z0).shiftLeft(middle) + z0);
}