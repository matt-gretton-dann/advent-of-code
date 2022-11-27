//
// Created by Matthew Gretton-Dann on 01/12/2021.
//

#include <cstdlib>
#include <iostream>
#include <string>

using Int = std::int64_t;

auto main() -> int
{
  Int value{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    Int incr{std::stoll(line)};
    value += incr;
  }
  std::cout << "Frequency: " << value << '\n';
  return EXIT_SUCCESS;
}
