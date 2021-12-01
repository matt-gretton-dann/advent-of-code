//
// Created by Matthew Gretton-Dann on 01/12/2021.
//

#include <cstdlib>
#include <limits>
#include <string>
#include <iostream>
#include <vector>

int main(void) {
    std::vector<unsigned long> nums;
    unsigned incrs{0};
    std::string line;
    while (std::getline(std::cin, line)) {
        nums.push_back(std::stoul(line));
    }
    assert(nums.size() >= 3);
    unsigned long prev{std::numeric_limits<unsigned long>::max()};
    for (std::size_t pos = 0; pos <= nums.size() - 3; ++pos) {
        unsigned long current{0};
        for (std::size_t i = pos + 0; i < pos + 3; ++i) {
            current += nums[i];
        }
        if (current > prev) {
            ++incrs;
        }
        prev = current;
    }
    std::cout << "Number of increments: " << incrs << '\n';
    return EXIT_SUCCESS;
}
