//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <iostream>
#include <numeric>
#include <string>
#include <vector>

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read input.\n";
    return 1;
  }

  std::vector<std::uint64_t> fish(9, 0);
  for (auto c : line) {
    if (c == ',') {
      continue;
    }
    if (c >= '0' && c <= '8') {
      ++fish.at(c - '0');
    }
    else {
      std::cerr << "Can't interpret " << c << '\n';
      return 1;
    }
  }

  constexpr unsigned days_wanted{256};
  for (unsigned day{0}; day < days_wanted; ++day) {
    std::vector<std::uint64_t> new_fish;
    std::copy(fish.begin() + 1, fish.end(), std::back_inserter(new_fish));
    new_fish.at(6) += fish.at(0);
    new_fish.push_back(fish.at(0));
    std::swap(fish, new_fish);
  }

  std::cout << "Fish count after " << days_wanted << " days: "
            << std::accumulate(fish.begin(), fish.end(), std::uint64_t{0},
                               [](auto a, auto b) { return a + b; })
            << '\n';
  return 0;
}