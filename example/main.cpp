#include "BigUint.h"
#include <iostream>

int main() {
    BigUint a;
    std::cout << "Initially a is: " << a.toString() << '\n';

    constexpr int32_t times = BigUint::BASE - 1;
    for (int32_t ii=0; ii<times; ii++) {
        a.plusOne();
    }
    std::cout << "a.plusOne() " << BigUint::BASE -1  << " times is: " << a.toString() << '\n';;
    a.plusOne();
    std::cout << "a.plusOne() one more time is: " << a.toString() << '\n';;
    a.minusOne();
    std::cout << "a.minusOne() is: " << a.toString() << '\n';;

    //std::cout << "In base 10 is: " << a.toBase10String() << std::endl;
    
    /*
    { 
        BigUint a(1000);
        const BigUint b(250);

        std::cout << a << "a = " << a << '\n';
        std::cout << b << "b = " << b << '\n';

        a += b;
        std::cout << "a += b: " << a << '\n'; // Outputs: 1250

        a -= b;
        std::cout << "a -= b: " << a << '\n'; // Outputs: 1000

        a *= b;
        std::cout << "a *= b: " << a << '\n'; // Outputs: 250000

        a /= b;
        std::cout << "a /= b: " << a << '\n'; // Outputs: 1000

        a %= BigUint(300);
        std::cout << "a %= 300: " << a << '\n'; // Outputs: 100
    }
    */

    /*
    {
        const BigUint base(5);
        const BigUint exponent(3);
        const BigUint mod(13);

        std::cout << "5^3 % 13: " << base.modPow(exponent, mod) << '\n';  // Outputs: 8 (5^3 % 13 = 125 % 13 = 8)
    }
    */

    /*
    {
        const BigUint a("123456789123456789");
        const BigUint b("987654321987654321");
        std::cout << a << "a = " << a << '\n';
        std::cout << b << "b = " << b << '\n';
        std::cout << a << " + " << b << " = " << a + b << '\n';
        std::cout << a << " - " << b << " = " << a - b << '\n';
        std::cout << b << " - " << a << " = " << b - a << '\n';
        std::cout << a << " * " << b << " = " << a * b << '\n';
        std::cout << a << " / " << b << " = " << a / b << '\n';
        std::cout << b << " / " << a << " = " << b / a << '\n';
        std::cout << a << " % " << b << " = " << a % b << '\n';
        std::cout << b << " % " << a << " = " << b % a << '\n';
    }
        */

    return 0;
}

