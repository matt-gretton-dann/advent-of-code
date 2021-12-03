//
// Created by Matthew Gretton-Dann on 03/12/2021.
//

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

auto main() -> int
{
  std::vector<unsigned> bit_counts;
  std::string line;
  unsigned line_count{0};
  while (std::getline(std::cin, line)) {
    ++line_count;
    if (bit_counts.empty()) {
      bit_counts.resize(line.size(), 0);
    }
    assert(bit_counts.size() == line.size());
    for (std::size_t i{0}; i < line.size(); ++i) {
      if (line[i] == '1') {
        bit_counts[i]++;
      }
    }
  }

  std::uint64_t gamma_rate{0};
  std::uint64_t epsilon_rate{0};
  for (auto count : bit_counts) {
    gamma_rate <<= 1;
    epsilon_rate <<= 1;
    if (count > line_count / 2) {
      gamma_rate |= 1;
    }
    else {
      epsilon_rate |= 1;
    }
  }

  std::cout << "Gamma rate * epsilon rate = " << gamma_rate << " * " << epsilon_rate << " = "
            << gamma_rate * epsilon_rate << '\n';
  return 0;
}