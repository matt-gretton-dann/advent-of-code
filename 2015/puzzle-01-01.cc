#include <cassert>
#include <iostream>
#include <string>

auto main() -> int
{
  for (std::string line; std::getline(std::cin, line);) {
    int floor = 0;
    for (auto c : line) {  // NOLINT(readability-identifier-length)
      assert(c == '(' || c == ')');
      floor += (c == '(') ? 1 : 0;
      floor -= (c == ')') ? 1 : 0;
    }
    std::cout << floor << "\n";
  }

  return 0;
}