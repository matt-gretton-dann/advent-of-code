//
// Created by Matthew Gretton-Dann on 04/12/2021.
//

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <vector>

auto main() -> int
{
  std::vector<std::array<unsigned, 26>> counts;
  std::string line;

  while (std::getline(std::cin, line)) {
    if (counts.empty()) {
      counts.resize(line.size());
      for (unsigned i = 0; i < line.size(); ++i) {
        for (unsigned j = 0; j < 26; ++j) {
          counts[i][j] = 0;
        }
      }
    }

    assert(counts.size() == line.size());
    for (std::size_t i = 0; i < line.size(); ++i) {
      ++counts[i][line.at(i) - 'a'];
    }
  }

  for (auto const& idx : counts) {
    auto letter{std::max_element(idx.begin(), idx.end())};  // NOLINT(llvm-qualified-auto)
    std::cout << static_cast<char>('a' + (letter - idx.begin()));
  }
  std::cout << '\n';

  return 0;
}