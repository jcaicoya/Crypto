#include <iostream>
#include "BigUint.h"
#include "performance_timer.h"

int main() {
    constexpr int numDigits = 1'000;
    const BigUint a(std::string(numDigits, '9'));
    const BigUint b(std::string(numDigits, '8'));

    const double addTime = measureExecutionTime([&]() { BigUint c = a + b; });
    const double subTime = measureExecutionTime([&]() { BigUint c = a - b; });
    const double divTime = measureExecutionTime([&]() { BigUint c = a / b; });
    const double modTime = measureExecutionTime([&]() { BigUint c = a % b; });

    std::cout << "Arithmetic Benchmark (" << numDigits << " digits):\n";
    std::cout << "Addition: " << addTime << " ms\n";
    std::cout << "Subtraction: " << subTime << " ms\n";
    std::cout << "Division: " << divTime << " ms\n";
    std::cout << "Modulus: " << modTime << " ms\n";

    return 0;
}
