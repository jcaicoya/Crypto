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
};

int main() {
    constexpr int numDigits = 1'000;
    const BigUint a(std::string(numDigits, '9'));
    const BigUint b(std::string(numDigits, '8'));

    const double naiveTime = measureExecutionTime([&]() { BigUint c = BigUintBenchmarkAccessor::multiplyNaive(a, b);});
    /*
    const double karatsubaTime = measureExecutionTime([&]() { BigUint c = a.multiplyKaratsuba(b); });
    const double fftTime = measureExecutionTime([&]() { BigUint c = a.multiplyFFT(b); });
    */

    std::cout << "Multiplication Benchmark (" << numDigits << " digits):\n";
    std::cout << "Naïve Multiplication: " << naiveTime << " ms\n";
    /*
    std::cout << "Karatsuba Multiplication: " << karatsubaTime << " ms\n";
    std::cout << "FFT Multiplication: " << fftTime << " ms\n";
    */

    return 0;
}