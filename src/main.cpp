#include <iostream>
#include <string>
#include <cctype>
#include <bits/ranges_algo.h>

bool isPrime(const unsigned long long n) {
    if (n < 2) {
        return false;
    }

    if (n == 2) {
        return true;
    }

    if (n % 2 == 0) {
        return false;
    }

    for (unsigned long long i = 3; i <= n / i; i += 2) {
        if (n % i == 0) {
            return false;
        }
    }

    return true;
}

bool isValidUnsignedInteger(const std::string& input) {
    if (input.empty()) {
        return false;
    }

    for (const char ch : input) {
        if (!std::isdigit(static_cast<unsigned char>(ch))) {
            return false;
        }
    }

    return true;
}

int main() {
    std::cout << "Prime number checker" << std::endl;
    std::cout << "Enter a non-negative integer: ";

    std::string input;
    std::getline(std::cin, input);

    if (std::cin.fail()) {
        std::cerr << "Input error." << std::endl;
        return 1;
    }

    if (!isValidUnsignedInteger(input)) {
        std::cerr << "Error: you must enter a non-negative integer." << std::endl;
        return 1;
    }

    unsigned long long number = 0;

    try {
        size_t pos = 0;
        number = std::stoull(input, &pos);

        if (pos != input.size()) {
            std::cerr << "Error: invalid characters in input." << std::endl;
            return 1;
        }
    } catch (...) {
        std::cerr << "Error: number is too large." << std::endl;
        return 1;
    }

    if (constexpr unsigned long long MAX_VALUE = 1000000000000ULL; number > MAX_VALUE) {
        std::cerr << "Error: number is too large. Maximum allowed value is "
                  << MAX_VALUE << "." << std::endl;
        return 1;
    }

    if (isPrime(number)) {
        std::cout << number << " is prime." << std::endl;
    } else {
        std::cout << number << " is not prime." << std::endl;
    }

    return 0;
}
