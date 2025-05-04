#include <iostream>
#include "BigUint.h"
#include "performance_timer.h"

class BigUintBenchmarkAccessor {
public:
    static const std::vector<BigUint::DigitType> & get_digits(const BigUint &bigUint) {
        return bigUint.digits_;
    }

    static [[nodiscard]] BigUint multiplyNaive(const BigUint &lhs, const BigUint& rhs) {
        return lhs.multiplyNaive(rhs);
    }

    static [[nodiscard]] BigUint multiplyKaratsuba(const BigUint &lhs, const BigUint& rhs) {
        return lhs.multiplyKaratsuba(rhs);
    }

    static [[nodiscard]] BigUint multiplyFFT(const BigUint &lhs, const BigUint& rhs) {
        return lhs.multiplyFFT(rhs);
    }
};

int main() {
    constexpr int numberOfDigits = 10;
    const BigUint a(BigUint::Digits(numberOfDigits, 9));
    const BigUint b(BigUint::Digits(numberOfDigits, 8));

    const double naiveTime = measureExecutionTime([&]() { BigUint c = BigUintBenchmarkAccessor::multiplyNaive(a, b);});
    const double karatsubaTime = measureExecutionTime([&]() { BigUint c = BigUintBenchmarkAccessor::multiplyKaratsuba(b, b); });
    const double fftTime = measureExecutionTime([&]() { BigUint c = BigUintBenchmarkAccessor::multiplyFFT(a, b); });

    std::cout << "Multiplication Benchmark (" << numberOfDigits << " digits):\n";
    std::cout << "Naïve Multiplication: " << naiveTime << " ms\n";
    std::cout << "Karatsuba Multiplication: " << karatsubaTime << " ms\n";
    std::cout << "FFT Multiplication: " << fftTime << " ms\n";

    return 0;
}