#include "BigUint.h"
#include <iostream>

std::ostream & print(const BigUint& a, std::ostream &out = std::cout) {
    out << a.to_base10_string() << " <--> " << a.to_string();
    return out;
}

std::ostream & partial_print(const std::string& str, std::size_t displayed_digits, std::ostream &out = std::cout) {
    const auto number_digits = str.length();
    if (number_digits <= 2 * displayed_digits) {
        out << str;
        return out;
    }

    const auto first_part = str.substr(0, displayed_digits);
    const auto last_part = str.substr(number_digits - displayed_digits);
    out << first_part << "..." << last_part;
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

    // Powers of two
    std::cout << "\nPowers of two:\n";
    constexpr uint16_t power = 65'534;
    constexpr std::size_t displayed_digits = 10;

    auto result = BigUint::TWO.pow_by(power).to_base10_string();
    std::cout << 2 << "^" << power << " has " << result.length() << " digits: ";
    partial_print(result, displayed_digits) << '\n';

    const BigUint base_as_power = BigUint::from_base10_string("65535");
    result = BigUint::TWO.pow_by(base_as_power).to_base10_string();
    std::cout << 2 << "^" << base_as_power.to_base10_string() << " has " << result.length() << " digits: ";
    partial_print(result, displayed_digits) << '\n';

    // Powers of three
    std::cout << "\nPowers of three:\n";
    const BigUint number_3{3};
    result = number_3.pow_by(power).to_base10_string();
    std::cout << 3 << "^" << power << " has " << result.length() << " digits: ";
    partial_print(result, displayed_digits) << '\n';

    result = number_3.pow_by(base_as_power).to_base10_string();
    std::cout << 3 << "^" << base_as_power.to_base10_string() << " has " << result.length() << " digits: ";
    partial_print(result, displayed_digits) << '\n';
    return 0;
}

