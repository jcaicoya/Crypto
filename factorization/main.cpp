#include <filesystem>

#include "BigUint.h"
#include <iostream>
#include <fstream>
#include <exception>
#include <__msvc_filebuf.hpp>

std::ostream & print(const BigUint& a, std::ostream &out = std::cout) {
    out << a.toBase10String() << " <--> " << a.toString();
    return out;
}

std::size_t count_number_of_lines(const std::filesystem::path &path) {
    std::ifstream fin(path, std::ios::binary); // Bynary mode for performance
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

using FactorTable = std::vector<std::vector<BigUint>>;
using PrimeNumbers = std::vector<BigUint>;

[[nodiscard]] FactorTable build_factor_table_from_file(const std::filesystem::path &path) {
    FactorTable factor_table;
    const auto number_of_lines = count_number_of_lines(path);
    factor_table.reserve(number_of_lines * 2 + 1);

    std::ifstream fin(path);
    if (!fin) {
        throw std::runtime_error("Could not open file " + path.string());
    }

    std::string line;
    std::size_t line_counter = 0;
    std::size_t table_position = 0;
    while (std::getline(fin, line)) {
        line_counter++;
        std::istringstream iss(line);
        iss >> std::noskipws;
        if (line.empty()) {
            std::cerr << "line " << line_counter << " is empty" << std::endl;
            continue;
        }

        std::vector<BigUint> table_line;
        while (!iss.eof()) {
            try {
                BigUint number;
                iss >> number;
                table_line.push_back(number);
            } catch (const std::exception &e) {
                throw std::runtime_error("Could not parse line " + std::to_string(line_counter) + ": " + e.what());
            }
        }

        factor_table.push_back(table_line);
        table_position++;
    }

    return factor_table;
}

void summarize_factor_table(const FactorTable &factor_table) {
    std::size_t table_line_counter = 0;
    for (const auto &number_and_factors : factor_table) {
        if (number_and_factors.empty()) {
            std::string error_message = "Factor table line ";
            error_message += std::to_string(table_line_counter);
            error_message += " is empty";
            throw std::runtime_error(error_message.c_str());
        }
        table_line_counter++;

        if (number_and_factors.size() == 1) {
            std::cout << number_and_factors.front() << " has not been factorized\n";
        }

        std::cout << number_and_factors.front();
        if (number_and_factors.size() == 2) {
            std::cout << " is prime\n";
        }
        else {
            std::cout << " = " << number_and_factors[1];
            for (std::size_t ii = 2; ii < number_and_factors.size(); ii++) {
                std::cout << " * " << number_and_factors[ii];
            }
            std::cout << '\n';
        }
    }
}

void load_primes_from_table(const FactorTable &factor_table, PrimeNumbers &prime_numbers) {
    for (auto &table_line: factor_table) {
        if (table_line.size() == 2) {
            prime_numbers.push_back(table_line.front());
        }
    }
}

int main() {
    std::filesystem::path path("resources/factorization.txt");
    FactorTable factor_table;
    try {
        factor_table = build_factor_table_from_file(path);
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
    return 0;
}

