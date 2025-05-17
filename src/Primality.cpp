#include "Primality.h"
#include <array>

namespace primality {

     constexpr std::array<uint8_t, 256> generate_decimal_digit_table() {
          std::array<uint8_t, 256> table{};
          for (uint16_t i = 0; i < 256; ++i) {
               table[i] = i % 10;
          }
          return table;
     }

     constexpr auto lsb_decimal_digits = generate_decimal_digit_table();

     uint8_t get_least_significant_byte(const BigUint &number) {
          const auto least_significant_digit = number.get_least_significant_digit();
          const uint8_t least_significant_byte = least_significant_digit & 0xFF;
          return least_significant_byte;
     }

     bool is_divisible_by(const BigUint &number, uint8_t divisor) {
          if (divisor == 0) {
               throw std::invalid_argument("divisor cannot be zero");
          }

          if (divisor == 1) {
               return true;
          }

          const auto least_significant_bit = get_least_significant_byte(number);
          const auto table_value = lsb_decimal_digits[least_significant_bit];
          return table_value == divisor;
     }

     bool is_divisible_by(const BigUint &number, const BigUint &divisor) {
          if (divisor == BigUint::ZERO) {
               throw std::invalid_argument("divisor cannot be zero");
          }

          if (divisor ==  BigUint::ONE) {
               return true;
          }

          const auto remainder = number % divisor;
          return remainder == BigUint::ZERO;
     }

     bool is_prime(const BigUint &/*number*/) {
          // TODO
          return false;
     }

} // end namespace primality
