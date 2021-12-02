#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

using Jolt = unsigned long;

Jolt count_valid_combinations(std::vector<Jolt> const& jolts)
{
  std::vector<Jolt> valid(jolts.size(), 0);
  valid[jolts.size() - 1] = 1;
  std::size_t i = jolts.size() - 1;
  do {
    auto i2 = i;
    --i;

    valid[i] = 0;
    while (i2 < jolts.size() && jolts[i2] < jolts[i] + 4) {
      valid[i] += valid[i2];
      ++i2;
    }
    std::cout << jolts[i] << ": " << valid[i] << "\n";
  } while (i > 0);
  return valid[0];
}

int main(int argc, char** argv)
{
  std::vector<Jolt> jolts;
  for (std::string line; std::getline(std::cin, line);) {
    jolts.push_back(std::stoul(line));
  }

  jolts.push_back(0);
  std::sort(jolts.begin(), jolts.end());
  jolts.push_back(jolts[jolts.size() - 1] + 3);

  auto combinations = count_valid_combinations(jolts);
  std::cout << "Number of valid combinations: " << combinations << "\n";
  return 0;
}