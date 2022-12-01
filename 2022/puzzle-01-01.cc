//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <string>

using Int = std::int64_t;

auto main() -> int
{
  Int current_value{0};
  Int max_value{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      max_value = std::max(max_value, current_value);
      current_value = 0;
    }
    else {
      Int incr{std::stoll(line)};
      current_value += incr;
    }
  }
  max_value = std::max(max_value, current_value);
  std::cout << "Max value: " << max_value << '\n';
  return EXIT_SUCCESS;
}
