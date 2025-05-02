#include <iostream>
#include "BigUint.h"
#include "performance_timer.h"


void measureArithmeticOperation(const int numberOfDigits) {
    const BigUint a(BigUint::Digits(numberOfDigits, 9));
    const BigUint b(BigUint::Digits(numberOfDigits, 8));

    const double addTime = measureExecutionTime([&]() { BigUint c = a + b; });
    const double subTime = measureExecutionTime([&]() { BigUint c = a - b; });
    const double divTime = measureExecutionTime([&]() { BigUint c = a / b; });
    const double modTime = measureExecutionTime([&]() { BigUint c = a % b; });

    std::cout << "Arithmetic benchmark for " << numberOfDigits << " digits:\n";
    std::cout << "Addition: " << addTime << " ms\n";
    std::cout << "Subtraction: " << subTime << " ms\n";
    std::cout << "Division: " << divTime << " ms\n";
    std::cout << "Modulus: " << modTime << " ms\n";
}


int main() {
    {
        constexpr int numberOfDigits = 100;
        measureArithmeticOperation(numberOfDigits);
        std::cout << std::endl;
    }

    {
        constexpr int numberOfDigits = 1'000;
        measureArithmeticOperation(numberOfDigits);
        std::cout << std::endl;
    }

    {
        constexpr int numberOfDigits = 10'000;
        measureArithmeticOperation(numberOfDigits);
        std::cout << std::endl;
    }
    return 0;
}
