//
// Created by Matthew Gretton-Dann on 01/12/2021.
//

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

auto main() -> int
{
  std::vector<std::uint64_t> nums;
  unsigned incrs{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    nums.push_back(std::stoul(line));
  }
  assert(nums.size() >= 3);
  std::uint64_t prev{std::numeric_limits<std::uint64_t>::max()};
  for (std::size_t pos = 0; pos <= nums.size() - 3; ++pos) {
    std::uint64_t current{0};
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
