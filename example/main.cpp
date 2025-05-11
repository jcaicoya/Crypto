#include "BigUint.h"
#include <iostream>

std::ostream & print(const BigUint& a, std::ostream &out = std::cout) {
    out << a.toBase10String() << " <--> " << a.toString();
    return out;
}

int main() {
    // Addition, substraction and from/to base 10.
    {
        BigUint a;
        std::cout << "Initially a is: "; print(a) << '\n';

        constexpr int32_t times = BigUint::BASE - 1;
        for (int32_t ii=0; ii<times; ii++) {
            a.mePlusOne();
        }
        std::cout << "a.mePlusOne() " << times  << " times is: "; print(a) << '\n';
        a.mePlusOne();
        std::cout << "a.mePlusOne() is: "; print(a) << '\n';
        a.mePlusOne();
        std::cout << "a.mePlusOne() is: "; print(a) << "\n\n";

        a.meMinusOne();
        std::cout << "a.meMinusOne() is: "; print(a) << '\n';
        a.meMinusOne();
        std::cout << "a.meMinusOne() is: "; print(a) << '\n';
        for (int32_t ii=0; ii<times; ii++) {
            a.meMinusOne();
        }
        std::cout << "a.meMinusOne() " << times  << " times is: "; print(a) << '\n';

        BigUint b = BigUint::fromBase10String("123456789");
        std::cout << "b is: "; print(b) << '\n';
        b.subtractToMe(b);
        std::cout << "After b.subtractToMe(b), b is: "; print(b) << "\n\n";
    }

    // Modular multiplication
    {
        const BigUint a = BigUint::fromBase10String("12345");
        const BigUint b = BigUint::fromBase10String("67890");
        const auto aMulb = a * b;
        std::cout << "a is: "; print(a) << '\n';
        std::cout << "b is: "; print(b) << '\n';
        std::cout << "a * b is: "; print(aMulb) << '\n';
        const BigUint n = BigUint::fromBase10String("10000");
        std::cout << "n is: "; print(n) << '\n';
        const auto result = BigUint::modMul(b, a, n); // multiplication is 838.102.050
        std::cout << "(a * b) mod(n) is: "; print(result) << '\n';
    }

    return 0;
}

