//
// Created by Matthew Gretton-Dann on 04/12/2021.
//

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

auto main() -> int
{
  std::vector<std::map<char, unsigned>> counts;
  std::string line;

  while (std::getline(std::cin, line)) {
    if (counts.empty()) {
      counts.resize(line.size());
    }

    assert(counts.size() == line.size());
    for (std::size_t i = 0; i < line.size(); ++i) {
      auto [it, success] = counts[i].insert({line[i], 0});
      ++(it->second);
    }
  }

  for (auto const& idx : counts) {
    auto letter{std::min_element(idx.begin(), idx.end(), [](auto const& lhs, auto const& rhs) {
      return lhs.second < rhs.second;
    })};
    std::cout << static_cast<char>(letter->first);
  }
  std::cout << '\n';

  return 0;
}