#include <filesystem>

#include "BigUint.h"
#include <iostream>
#include <fstream>
#include <exception>
#include <map>

std::ostream & print(const BigUint& a, std::ostream &out = std::cout) {
    out << a.to_base10_string() << " <--> " << a.to_string();
    return out;
}

std::size_t count_number_of_lines(const std::filesystem::path &path) {
    std::ifstream fin(path, std::ios::binary); // Binary mode for performance
    if (!fin) {
        throw std::runtime_error("Could not open file " + path.string());
    }

    constexpr std::size_t buffer_size = 4'096;
    char buffer[buffer_size];
    std::size_t line_counter = 0;

    while (fin.read(buffer, buffer_size) || fin.gcount() > 0) {
        std::streamsize bytes_read = fin.gcount();
        for (std::streamsize ii = 0; ii < bytes_read; ii++) {
            if (buffer[ii] == '\n') {
                line_counter++;
            }
        }
    }

    fin.close();
    return line_counter;
}

using FactorTable = std::map<BigUint, std::vector<BigUint>>;
using PrimeNumbers = std::vector<BigUint>;

[[nodiscard]] FactorTable build_factor_table_from_file(const std::filesystem::path &path) {
    std::ifstream fin(path);
    if (!fin) {
        throw std::runtime_error("Could not open file " + path.string());
    }

    FactorTable factor_table;
    std::string line;
    std::size_t line_counter = 0;
    while (std::getline(fin, line)) {
        line_counter++;
        std::istringstream iss(line);
        iss >> std::noskipws;
        if (line.empty()) {
            std::cerr << "line " << line_counter << " is empty" << std::endl;
            continue;
        }

        BigUint number;
        std::vector<BigUint> factors;
        iss >> number;
        while (!iss.eof()) {
            try {
                BigUint factor;
                iss >> factor;
                factors.emplace_back(factor);
            } catch (const std::exception &e) {
                throw std::runtime_error("Could not parse line " + std::to_string(line_counter) + ": " + e.what());
            }
        }

        factor_table.emplace(number, factors);
    }

    return factor_table;
}

void print_number_and_its_factors(const BigUint &number, const std::vector<BigUint> &factors) {
    std::cout << number << ' ';
    if (factors.empty()) {
        std::cout << " is prime";
    }
    else {
        std::cout << " = " << factors[0];
        for (std::size_t ii = 1; ii < factors.size(); ii++) {
            std::cout << " * " << factors[ii];
        }
    }
}

void summarize_factor_table(const FactorTable &factor_table) {
    for (const auto &[number, factors] : factor_table) {
        print_number_and_its_factors(number, factors);
        std::cout << '\n';
    }
}

void load_primes_from_table(const FactorTable &factor_table, PrimeNumbers &prime_numbers) {
    for (const auto &[number, factors] : factor_table) {
        if (factors.empty()) {
            prime_numbers.push_back(number);
        }
    }
}

BigUint next_prime(const PrimeNumbers &prime_numbers, const BigUint &prime) {
    auto itr = prime_numbers.begin();
    while (*itr != prime && itr != prime_numbers.end()) {
        ++itr;
    }

    if (itr == prime_numbers.end()) {
        std::string message = "Could not find prime ";
        message += prime.to_base10_string();
        throw std::runtime_error(message.c_str());
    }

    ++itr;
    if (itr == prime_numbers.end()) {
        std::string message = "Could not get next prime to ";
        message += prime.to_base10_string();
        throw std::runtime_error(message.c_str());
    }

    return *itr;
}

std::vector<BigUint> factorize(const BigUint &number, const FactorTable &factor_table, const PrimeNumbers &prime_numbers) {
    if (number == BigUint::ZERO or number == BigUint::ONE) {
        throw std::runtime_error("number cannot be zero or one");
    }

    const auto itr = factor_table.find(number);
    if (itr != factor_table.end()) {
        return itr->second;
    }

    BigUint possible_divisor = BigUint::TWO;
    BigUint possible_quotient = BigUint::ZERO;
    bool found_divisor = false;
    while (!found_divisor && possible_divisor.square() <= number) {
        const auto [quotient, remainder] = number.divide_by(possible_divisor);
        if (remainder == BigUint::ZERO) {
            found_divisor = true;
            possible_quotient = quotient;
        }
        else {
            possible_divisor = next_prime(prime_numbers, possible_divisor);
        }
    }

    if (!found_divisor) {
        return {};
    }

    auto factors = factor_table.at(possible_quotient);
    if (factors.empty()) {
        factors.emplace_back(possible_quotient);
    }
    factors.emplace_back(possible_divisor);
    std::ranges::sort(factors);
    return factors;
}

void write_number_and_factors_at_the_end_of_file(const BigUint &number, const std::vector<BigUint> &factors, const std::filesystem::path &path) {
    std::ofstream out(path, std::ios::app);
    if (!out) {
        throw std::runtime_error("Could not open file " + path.string());
    }

    out << '\n' << number;
    if (!factors.empty()) {
        for (const auto &factor : factors) {
            out << ' ' << factor;
        }
    }

    out.close();
}

int main() {
    const std::filesystem::path resource_dir_path = RSC_PATH;
    const std::filesystem::path file_path = resource_dir_path / "dev-factorization.txt";
    std::cout << "Working with " << file_path.string() << "\n";
    std::cout << std::endl;

    FactorTable factor_table;
    try {
        factor_table = build_factor_table_from_file(file_path);
    } catch (const std::exception &e) {
        std::cout << e.what() << '\n';
    }

    std::cout << '\n';
    std::cout << "Factorization table:\n";
    std::cout << "--------------------\n";
    summarize_factor_table(factor_table);

    PrimeNumbers prime_numbers;
    load_primes_from_table(factor_table, prime_numbers);
    std::cout << '\n';
    std::cout << "Prime numbers:\n";
    std::cout << "--------------\n";
    for (auto &prime_number : prime_numbers) {
        std::cout << prime_number << '\n';
    }

    std::cout << '\n';
    std::cout << "Factoring...\n";
    std::cout << "------------\n";
    constexpr int number_of_steps = 1'000;
    std::chrono::duration<double, std::milli> factoring_duration{};
    for (int steps = 1; steps <= number_of_steps; steps++) {
        BigUint number = factor_table.rbegin()->first;
        number.me_plus_one();
        const auto start = std::chrono::high_resolution_clock::now();
        const auto factors = factorize(number, factor_table, prime_numbers);
        auto end = std::chrono::high_resolution_clock::now();
        factoring_duration += (end - start);
        factor_table.emplace_hint(factor_table.end(), number, factors);
        if (factors.empty()) {
            prime_numbers.push_back(number);
        }
        write_number_and_factors_at_the_end_of_file(number, factors, file_path);
        print_number_and_its_factors(number, factors);
        std::cout << '\n';
    }

    std::cout << '\n' << number_of_steps << " numbers has been factorized in " << factoring_duration.count() << " ms\n";

    return 0;
}

