#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Jolt = unsigned long;

int main(int argc, char **argv) {
  std::vector<Jolt> jolts;
  for (std::string line; std::getline(std::cin, line);) {
    jolts.push_back(std::stoul(line));
  }

  std::sort(jolts.begin(), jolts.end());
  std::array<unsigned, 4> gaps = {};

  Jolt last = 0;
  for (auto j : jolts) {
    ++gaps[j - last];
    last = j;
  }
  // and the gap to our voltage
  ++gaps[3];

  std::cout << gaps[1] << " * " << gaps[3] << " = " << gaps[1] * gaps[3]
            << "\n";
  return 0;
}