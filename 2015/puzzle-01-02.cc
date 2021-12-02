#include <cassert>
#include <iostream>
#include <string>

auto main() -> int
{
  for (std::string line; std::getline(std::cin, line);) {
    int floor = 0;
    std::string::size_type pos = 0;
    for (auto c : line) {  // NOLINT(readability-identifier-length)
      ++pos;
      assert(c == '(' || c == ')');
      floor += (c == '(') ? 1 : 0;
      floor -= (c == ')') ? 1 : 0;
      if (floor == -1) {
        break;
      }
    }
    std::cout << pos << "\n";
  }

  return 0;
}