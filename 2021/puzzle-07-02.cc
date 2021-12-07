//
// Created by Matthew Gretton-Dann on 07/12/2021.
//

#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

auto main() -> int
{
  std::vector<unsigned> crabs;
  std::string line;
  while (std::getline(std::cin, line)) {
    std::size_t idx{0};
    while (idx < line.size()) {
      if (line[idx] == ',') {
        ++idx;
      }
      else {
        std::size_t end{0};
        crabs.emplace_back(std::stoul(line.substr(idx), &end));
        idx += end;
      }
    }
  }

  unsigned min_pos{0};
  unsigned min_fuel{std::numeric_limits<unsigned>::max()};
  auto [low_crab, high_crab] = std::minmax_element(crabs.begin(), crabs.end());
  for (auto pos{*low_crab}; pos <= *high_crab; ++pos) {
    auto fuel{std::accumulate(crabs.begin(), crabs.end(), unsigned{0}, [pos](auto a, auto b) {
      unsigned dist{(b > pos) ? (b - pos) : (pos - b)};
      return a + (dist * (dist + 1)) / 2;
    })};
    if (fuel < min_fuel) {
      min_fuel = fuel;
      min_pos = pos;
    }
  }

  std::cout << "Minimum fuel = " << min_fuel << " at position " << min_pos << '\n';
  return 0;
}