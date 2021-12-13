#include <iostream>
#include <string>
#include <vector>

using Int = long;

auto main() -> int
{
  std::vector<Int> jumps;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    jumps.push_back(std::stol(line));
  }

  Int pc{0};
  unsigned steps{0};
  while (pc >= 0 && pc < jumps.size()) {
    ++steps;
    auto delta = jumps[pc];
    ++(jumps[pc]);
    pc += delta;
  }

  std::cout << "Steps: " << steps << '\n';
  return 0;
}
