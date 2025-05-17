#include "BigUint.h"
#include <iostream>

std::ostream & print(const BigUint& a, std::ostream &out = std::cout) {
    out << a.to_base10_string() << " <--> " << a.to_string();
    return out;
}

int main() {
    // Addition, substraction and from/to base 10.
    {
        BigUint a;
        std::cout << "Initially a is: "; print(a) << '\n';

        constexpr int32_t times = BigUint::BASE - 1;
        for (int32_t ii=0; ii<times; ii++) {
            a.me_plus_one();
        }
        std::cout << "a.mePlusOne() " << times  << " times is: "; print(a) << '\n';
        a.me_plus_one();
        std::cout << "a.mePlusOne() is: "; print(a) << '\n';
        a.me_plus_one();
        std::cout << "a.mePlusOne() is: "; print(a) << "\n\n";

        a.me_minus_one();
        std::cout << "a.meMinusOne() is: "; print(a) << '\n';
        a.me_minus_one();
        std::cout << "a.meMinusOne() is: "; print(a) << '\n';
        for (int32_t ii=0; ii<times; ii++) {
            a.me_minus_one();
        }
        std::cout << "a.meMinusOne() " << times  << " times is: "; print(a) << '\n';

        BigUint b = BigUint::from_base10_string("123456789");
        std::cout << "b is: "; print(b) << '\n';
        b.subtract_me(b);
        std::cout << "After b.subtractToMe(b), b is: "; print(b) << "\n\n";
    }

    // Modular multiplication
    {
        const BigUint a = BigUint::from_base10_string("12345");
        const BigUint b = BigUint::from_base10_string("67890");
        const auto aMulb = a * b;
        std::cout << "a is: "; print(a) << '\n';
        std::cout << "b is: "; print(b) << '\n';
        std::cout << "a * b is: "; print(aMulb) << '\n';
        const BigUint n = BigUint::from_base10_string("10000");
        std::cout << "n is: "; print(n) << '\n';
        const auto result = BigUint::mod_mul(b, a, n); // multiplication is 838.102.050
        std::cout << "(a * b) mod(n) is: "; print(result) << '\n';
    }

    return 0;
}

