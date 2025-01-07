#include "BigInt.h"
#include <iostream>

int main() {
    {
        BigInt a("123456789123456789");
        BigInt b("987654321987654321");

        BigInt c = a + b; // Test addition
        std::cout << a << " + " << b << " = " << c << std::endl;
    }

    {
        BigInt a("12345");
        BigInt b = -a;

        std::cout << "a: " << a << std::endl; // Outputs: a: 12345
        std::cout << "b: " << b << std::endl; // Outputs: b: -12345

        BigInt c = BigInt(0);
        std::cout << "c: " << -c << std::endl; // Outputs: c: 0
    }

    {
        BigInt a("-12345");
        BigInt b = a.abs();

        std::cout << "a: " << a << std::endl;  // Outputs: a: -12345
        std::cout << "b: " << b << std::endl;  // Outputs: b: 12345

        BigInt c("67890");
        BigInt d = c.abs();

        std::cout << "c: " << c << std::endl;  // Outputs: c: 67890
        std::cout << "d: " << d << std::endl;  // Outputs: d: 67890
    }

    {
        BigInt a("1000");
        BigInt b("250");

        a += b;
        std::cout << "a += b: " << a << std::endl; // Outputs: 1250

        a -= b;
        std::cout << "a -= b: " << a << std::endl; // Outputs: 1000

        a *= b;
        std::cout << "a *= b: " << a << std::endl; // Outputs: 250000

        a /= b;
        std::cout << "a /= b: " << a << std::endl; // Outputs: 1000

        a %= BigInt("300");
        std::cout << "a %= 300: " << a << std::endl; // Outputs: 100
    }

    return 0;
}

