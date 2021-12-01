//
// Created by Matthew Gretton-Dann on 01/12/2021.
//

#include <cstdlib>
#include <limits>
#include <string>
#include <iostream>

int main(void) {
    unsigned long prev{std::numeric_limits<unsigned long>::max()};
    unsigned incrs{0};
    std::string line;
    while (std::getline(std::cin, line)) {
        auto current{std::stoul(line)};
        if (current > prev) {
            ++incrs;
        }
        prev = current;
    }
    std::cout << "Number of increments: " << incrs << '\n';
    return EXIT_SUCCESS;
}
