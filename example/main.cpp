#include "BigUint.h"
#include <iostream>

int main() {
    BigUint a;
    std::cout << "Initially a is: " << a.toString() << '\n';

    constexpr int32_t times = BigUint::BASE - 1;
    for (int32_t ii=0; ii<times; ii++) {
        a.mePlusOne();
    }
    std::cout << "a.mePlusOne() " << times  << " times is: " << a.toBase10String() << " <--> " << a.toString() << '\n';
    a.mePlusOne();
    std::cout << "a.mePlusOne() is: " << a.toBase10String() << " <--> " << a.toString() << '\n';
    a.mePlusOne();
    std::cout << "a.mePlusOne() is: " << a.toBase10String() << " <--> " << a.toString() << "\n\n";

    a.meMinusOne();
    std::cout << "a.meMinusOne() is: " << a.toBase10String() << " <--> " << a.toString() << '\n';
    a.meMinusOne();
    std::cout << "a.meMinusOne() is: " << a.toBase10String() << " <--> " << a.toString() << '\n';
    for (int32_t ii=0; ii<times; ii++) {
        a.meMinusOne();
    }
    std::cout << "a.meMinusOne() " << times  << " times is: " << a.toBase10String() << " <--> " << a.toString() << '\n';

    BigUint b = BigUint::fromBase10String("123456789");
    std::cout << "b is: " << b.toBase10String() << " <--> " << b.toString() << '\n';
    b.subtractToMe(b);
    std::cout << "After b.subtractToMe(b), b is: " << b.toBase10String() << " <--> " << b.toString() << '\n';

    return 0;
}

