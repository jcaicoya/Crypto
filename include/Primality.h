#ifndef PRIMALITY_H
#define PRIMALITY_H

#include "BigUint.h"

namespace primality
{
    bool is_divisible_by(const BigUint &number, uint8_t divisor);
    bool is_divisible_by(const BigUint &number, const BigUint &divisor);
    bool is_prime(const BigUint &number);
} // end namespace primality

#endif //PRIMALITY_H
