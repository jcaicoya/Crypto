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

void load_factors_from_file(const std::filesystem::path &path, FactorTable &table) {
    const auto number_of_lines = count_number_of_lines(path);
    table.resize(number_of_lines * 2 + 1);

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
            BigUint number;
            try {
                iss >> number;
                table_line.push_back(number);
            } catch (const std::exception &e) {
                throw std::runtime_error("Could not parse line " + std::to_string(line_counter) + ": " + e.what());
            }
        }

        table[table_position] = table_line;
        table_position++;
    }
}

void summarize_factor_table(const FactorTable &factor_table) {
    for (const auto &line : factor_table) {
        if (line.empty()) {
            return;
        }

        std::cout << line.front();
        if (line.size() == 1) {
            std::cout << " is prime\n";
        }
        else {
            std::cout << " = " << line[1];
            for (std::size_t ii = 1; ii < line.size(); ii++) {
                std::cout << " * " << line[ii];
            }
            std::cout << '\n';
        }
    }
}

void load_primes_from_table(const FactorTable &factor_table, PrimeNumbers &prime_numbers) {
    for (auto &table_line: factor_table) {
        if (table_line.size() == 1) {
            prime_numbers.push_back(table_line.front());
        }
    }
}


int main() {
    std::filesystem::path path("resources/factorization.txt");
    FactorTable factor_table;
    try {
        load_factors_from_file(path, factor_table);
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

