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

BigUint::BigUint(WideDigit digit) {
    if (digit < BigUint::BASE) {
        digits_.push_back(static_cast<DigitType>(digit));
        return;
    }

    const auto quotient = digit / BigUint::BASE;
    const auto remainer = digit - quotient * BigUint::BASE;
    digits_.push_back(static_cast<DigitType>(remainer));
    digits_.push_back(static_cast<DigitType>(quotient));
}

BigUint::BigUint(const std::string& str) {
    from_string(str);
}

BigUint::BigUint(const std::vector<DigitType> &digits)
    : digits_(digits) {
    if (digits_.empty()) {
        digits_.push_back(static_cast<DigitType>(0));
    }

    std::ranges::reverse(digits_.begin(), digits_.end());
}

std::optional<BigUint::DigitType> BigUint::as_digit() const {
    if (digits_.size() == 1) {
        return digits_[0];
    }

    return std::nullopt;
}

std::optional<BigUint::WideDigitType> BigUint::as_wide_digit() const {
    if (digits_.size() == 1) {
        return static_cast<WideDigitType>(digits_[0]);
    }

    if (digits_.size() == 2) {
        const WideDigitType wideDigit = digits_[0] + digits_[1] * BigUint::BASE;
        return wideDigit;
    }

    return std::nullopt;
}

std::optional<BigUint::ByteType> BigUint::as_byte_digit() const {
    if (digits_.size() > 1) {
        return std::nullopt;
    }

    const auto digit = digits_[0];
    if (digit >= 256) {
        return std::nullopt;
    }

    return static_cast<ByteType>(digit);
}

void BigUint::set_digits(const Digits &digits) {
    digits_ = digits;
    remove_leading_zeros();
    if (digits_.empty()) {
        *this = BigUint::ZERO;
    };
}

std::string BigUint::to_string() const {
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

BigUint BigUint::from_string(const std::string &str) {
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
            throw std::runtime_error("Invalid digit in BigUint string.");
        }
    }

    std::ranges::reverse(digits_.begin(), digits_.end());
    remove_leading_zeros();
    return *this;
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

void BigUint::shift_me_left(const size_t shiftPositions) {
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

BigUint BigUint::shift_left(const size_t shiftPositions) const {
    BigUint result = *this;
    result.shift_me_left(shiftPositions);
    return result;
}

void BigUint::me_plus_one() {
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

BigUint BigUint::plus_one() const {
    auto result = *this;
    result.me_plus_one();
    return result;
}

void BigUint::me_minus_one() {
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

BigUint BigUint::minus_one() const {
    auto result = *this;
    result.me_minus_one();
    return result;
}

void BigUint::add_me(DigitType digit) {
    if (digit == 0) {
        return;
    }

    if (*this == BigUint::ZERO) {
        digits_[0] = digit;
        return;
    }

    if (digit == 1) {
        me_plus_one();
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
    result.add_me(digit);
    return result;
}

void BigUint::operator+=(DigitType digit) {
    add_me(digit);
}

BigUint BigUint::operator+(DigitType digit) const {
    return add(digit);
}

void BigUint::add_me(const BigUint &rhs) {
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
    result.add_me(rhs);
    return result;
}

void BigUint::operator+=(const BigUint &rhs) {
    add_me(rhs);
}

BigUint BigUint::operator+(const BigUint &rhs) const {
    return add(rhs);
}

void BigUint::subtract_me(DigitType digit) {
    if (*this < digit) {
        throw std::runtime_error("Invalid negative result expected!");
    }

    if (*this == digit) {
        *this = BigUint::ZERO;
        return;
    }

    if (digit == 0) {
        return;
    }

    if (digit == 1) {
        me_minus_one();
        return;
    }

    if (digits_.size() == 1) {
        digits_[0] -= digit;
        return;
    }

    DigitType carry = digit;
    std::size_t digit_position = 0;
    while (digit_position < digits_.size() && carry != 0) {
        auto &current_digit = digits_[digit_position];
        if (current_digit >= carry) {
            current_digit -= carry;
            carry = 0;
        }
        else {
            current_digit = static_cast<DigitType>(BigUint::BASE - carry + current_digit);
            carry = 1;
            digit_position++;
        }
    }

    remove_leading_zeros();
}

BigUint BigUint::subtract(DigitType digit) const {
    BigUint result = *this;
    result.subtract_me(digit);
    return result;
}

void BigUint::operator-=(DigitType digit) {
    subtract_me(digit);
}

BigUint BigUint::operator-(DigitType digit) const {
    return subtract(digit);
}

void BigUint::subtract_me(const BigUint &rhs) {
    if (*this < rhs) {
        throw std::runtime_error("Invalid negative result expected!");
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
    remove_leading_zeros();
}

BigUint BigUint::subtract(const BigUint &rhs) const {
    auto result = *this;
    result.subtract_me(rhs);
    return result;
}

void BigUint::operator-=(const BigUint &rhs) {
    return subtract_me(rhs);
}

BigUint  BigUint::operator-(const BigUint &rhs) const {
    return subtract(rhs);
}

void BigUint::multiply_me_by(DigitType digit) {
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

BigUint BigUint::multiply_by(const DigitType digit) const {
    BigUint result = *this;
    result.multiply_me_by(digit);
    return result;
}

void BigUint::operator*=(const DigitType digit) {
    multiply_me_by(digit);
}

BigUint BigUint::operator*(const DigitType digit) const {
    return multiply_by(digit);
}

void BigUint::multiply_me_by(const BigUint &rhs) {
    *this = multiply_me_naive(rhs);
}

BigUint BigUint::multiply_by(const BigUint &rhs) const {
    BigUint result = *this;
    result.multiply_me_by(rhs);
    return result;
}

void BigUint::operator*=(const BigUint &rhs) {
    multiply_me_by(rhs);
}

BigUint BigUint::operator*(const BigUint &rhs) const {
    return multiply_by(rhs);
}

void BigUint::square_me() {
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

    const auto resultDigits = opt_inner_square(digits_);
    BigUint result;
    result.digits_ = resultDigits;
    return result;
}

void BigUint::pow_me_by(DigitType power) {
    *this = pow_by(power);
}

[[nodiscard]] BigUint BigUint::pow_by(DigitType power) const {
    if (*this == BigUint::ZERO) {
        if (power == 0) {
            throw std::runtime_error("zero pow by zero");
        }

        return BigUint::ONE;
    }

    if (*this == BigUint::ONE) {
        return BigUint::ONE;
    }

    BigUint u = BigUint::ONE;
    BigUint v = BigUint::ONE;
    BigUint z = *this;
    while (power > 0) {
        if (not ((power & 1) == 0)) {
            u = u * z;
        }
        power /= 2;
        v *= 2;
        z.square_me();
    }
    return u;
}

void BigUint::pow_me_by(const BigUint &power) {
    *this = pow_by(power);
}

[[nodiscard]] BigUint BigUint::pow_by(const BigUint &power) const {
    if (power.digits_.size() == 1) {
        return pow_by(power.digits_.front());
    }

    if (*this == BigUint::ZERO) {
        if (power == 0) {
            throw std::runtime_error("zero pow by zero");
        }

        return BigUint::ONE;
    }

    if (*this == BigUint::ONE) {
        return BigUint::ONE;
    }

    BigUint u = BigUint::ONE;
    BigUint v = BigUint::ONE;
    BigUint z = *this;
    BigUint n = power;
    while (n > 0) {
        if (n.is_odd()) {
            u = u * z;
        }
        n /= BigUint::TWO;
        v *= BigUint::TWO;
        z.square_me();
    }
    return u;
}

// returns the remainder
BigUint::DigitType BigUint::divide_me_by(const DigitType divisor) {
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
std::pair<BigUint, BigUint::DigitType> BigUint::divide_by(const DigitType divisor) const {
    BigUint quotient = *this;
    auto remainder = quotient.divide_me_by(divisor);
    return {quotient, remainder};
}

// returns the quotient
BigUint BigUint::operator/=(DigitType digit) {
    divide_me_by(digit);
    return *this;
}

// returns the quotient
BigUint BigUint::operator/(DigitType digit) const {
    const auto [quotient, remainder] = divide_by(digit);
    return quotient;
}

// returns the remainder
BigUint::DigitType BigUint::operator%=(DigitType digit) {
    return divide_me_by(digit);
}

// returns the remainder
BigUint::DigitType BigUint::operator%(DigitType digit) const {
    const auto [quotient, remainder] = divide_by(digit);
    return remainder;
}

// returns the remainder
BigUint BigUint::divide_me_by(const BigUint &rhs) {
    const auto [quotient, remainder] = divide_by(*this, rhs);
    *this = quotient;
    return remainder;
}

// returns quotient and remainder
[[nodiscard]] std::pair<BigUint, BigUint> BigUint::divide_by(const BigUint &rhs) const {
    return divide_by(*this, rhs);
}

// returns the quotient
BigUint BigUint::operator/=(const BigUint &rhs) {
    const auto [quotient, _] = divide_by(*this, rhs);
    *this = quotient;
    return quotient;
}

// returns the quotient
BigUint BigUint::operator/(const BigUint &rhs) const {
    const auto [quotient, _] = divide_by(rhs);
    return quotient;
}

// returns the remainder
BigUint BigUint::operator%=(const BigUint &rhs) {
    return divide_me_by(rhs);
}

// returns the remainder
[[nodiscard]] BigUint BigUint::operator%(const BigUint &rhs) const {
    const auto [_, remainder] = divide_by(rhs);
    return remainder;
}

std::string BigUint::to_base10_string() const {
    if (*this == ZERO) return "0";
    if (*this == ONE) return "1";
    if (*this == TWO) return "2";

    if (digits_.size() == 1) {
        return std::to_string(digits_[0]);
    }

    BigUint value = *this;
    std::string result;
    while (value.digits_.size() > 1) {
        const auto [quotient, remainder] = value.divide_by(10);
        result += static_cast<char>('0' + remainder);
        value = quotient;
    }

    std::ranges::reverse(result.begin(), result.end());
    return std::to_string(value.digits_[0]) + result;
}

BigUint BigUint::from_base10_string(const std::string& str) {
    if (str.empty()) throw std::runtime_error("Empty string is not a valid number.");

    BigUint result = BigUint::ZERO;
    for (auto d : str) {
        if (!std::isdigit(d)) throw std::runtime_error("Building BigUint: Invalid character");
        result.multiply_me_by(10);
        result.add_me(BigUint(static_cast<Digit>(d) - static_cast<Digit>('0')));
    }

    return result;
}

BigUint BigUint::mod_add(const BigUint& lhs, const BigUint& rhs, const BigUint& mod) {
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

BigUint BigUint::mod_sub(const BigUint& lhs, const BigUint& rhs, const BigUint& mod) {
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

BigUint BigUint::mod_mul(const BigUint& lhs, const BigUint& rhs, const BigUint& mod) {
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
        throw std::runtime_error("Modulo by zero is not allowed");
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

void BigUint::remove_leading_zeros() {
    while (digits_.size() > 1 && digits_.back() == 0) {
        digits_.pop_back();
    }

    if (digits_.empty()) {
        digits_.push_back(0);
    }
}

BigUint BigUint::multiply_me_naive(const BigUint& other) const {
    if (other.digits_.size() == 1) {
        return multiply_by(other.digits_.front());
    }

    if (digits_.size() == 1) {
        return other.multiply_by(digits_.front());
    }

    std::vector<BigUint> partialMultiplications(other.digits_.size());
    std::size_t shiftCounter = 0;
    for (const auto otherCurrentDigit : other.digits_) {
        BigUint currentMultiplication = this->multiply_by(otherCurrentDigit);
        currentMultiplication.shift_me_left(shiftCounter);
        partialMultiplications[shiftCounter] = currentMultiplication;
        shiftCounter++;
    }

    BigUint result;
    for (const auto &multiplication : partialMultiplications) {
        result += multiplication;
    }

    return result;
}

std::pair<BigUint, BigUint> BigUint::divide_by(const BigUint &dividend, const BigUint &divisor) {
    if (divisor == BigUint::ZERO) {
        throw std::runtime_error("Division by zero is not allowed.");
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
        const auto [quotient, remainder] = dividend.divide_by(digit);
        const BigUint remainderAsUint(remainder);
        return {quotient, remainderAsUint};
    }

    BigUint quotient, remainder;
    remainder.digits_.resize(dividend.digits_.size());

    // Process digits from most to least significant
    for (int i = static_cast<int>(dividend.digits_.size()) - 1; i >= 0; --i) {
        remainder.digits_.insert(remainder.digits_.begin(), dividend.digits_[i]);  // Shift remainder
        remainder.remove_leading_zeros();

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

    quotient.remove_leading_zeros();
    remainder.remove_leading_zeros();
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

BigUint BigUint::multiply_me_fft(const BigUint& b) const {
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
    high.remove_leading_zeros();
    return {low, high};
}

// NOLINTNEXTLINE(misc-no-recursion)
BigUint BigUint::multiply_me_karatsuba(const BigUint& other) const {
    if (constexpr std::size_t minimumNumberOfDigits = 2; digits_.size() < minimumNumberOfDigits || other.digits_.size() < minimumNumberOfDigits) {
        return multiply_me_naive(other);
    }

    const size_t middle = digits_.size() / 2;
    const auto [low1, high1] = split(middle);
    const auto [low2, high2] = other.split(middle);

    const BigUint z0 = low1.multiply_me_karatsuba(low2);
    const BigUint z1 = (low1 + high1).multiply_me_karatsuba(low2 + high2);
    const BigUint z2 = high1.multiply_me_karatsuba(high2);

    return (z2.shift_left(2 * middle) + (z1 - z2 - z0).shift_left(middle) + z0);
}

[[nodiscard]] std::vector<BigUint::DigitType> BigUint::opt_inner_square(const std::vector<BigUint::DigitType> &digits) {
    const size_t n = digits.size();
    std::vector<BigUint::WideDigitType> wideDigits(2 * n, static_cast<BigUint::WideDigit>(0));

    for (size_t ii = 0; ii < n; ii++) {
        // a_i^2 goes to position 2*i
        BigUint::WideDigit square = static_cast<BigUint::WideDigit>(digits[ii]) * digits[ii];
        if (square < BigUint::BASE) {
            wideDigits[2 * ii] = square;
        }
        else {
            const auto carry = square / BigUint::BASE;
            wideDigits[2 * ii] = square - carry * BigUint::BASE;
            wideDigits[2 * ii + 1] = carry;
        }
    }

    // Cross terms: 2 * a_i * a_j (i < j)
    for (size_t ii = 0; ii < n; ii++) {
        for (size_t jj = ii + 1; jj < n; jj++) {
            auto product = 2ULL * digits[ii] * digits[jj];

            size_t pos = ii + jj;
            auto sum = wideDigits[pos] + product;
            wideDigits[pos] = sum % BigUint::BASE;
            uint64_t carry = sum / BigUint::BASE;

            // Propagate carry forward if needed
            while (carry != 0) {
                pos++;
                sum = wideDigits[pos] + carry;
                wideDigits[pos] = sum % BigUint::BASE;
                carry = sum / BigUint::BASE;
            }
        }
    }

    if (wideDigits.back() == static_cast<BigUint::WideDigit>(0)) {
        wideDigits.pop_back();
    }

    std::vector<BigUint::DigitType> resultDigits(wideDigits.size());
    for (std::size_t jj = 0; jj < resultDigits.size(); jj++) {
        resultDigits[jj] = static_cast<BigUint::DigitType>(wideDigits[jj]);
    }
    return resultDigits;
}

std::ostream& operator<<(std::ostream& os, const BigUint& bigUint) {
    return os << bigUint.to_base10_string();
}

std::istream& operator>>(std::istream& is, BigUint& bigUint) {
    is >> std::ws;
    std::string number;
    if (!(is >> number)) {
        is.setstate(std::ios::failbit);
    }
    bigUint = BigUint::from_base10_string(number);
    return is;
}
