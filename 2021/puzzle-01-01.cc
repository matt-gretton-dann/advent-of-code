//
// Created by Matthew Gretton-Dann on 01/12/2021.
//

#include <cstdlib>
#include <iostream>
#include <limits>
#include <string>

auto main() -> int
{
  auto prev{std::numeric_limits<std::uint64_t>::max()};
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
