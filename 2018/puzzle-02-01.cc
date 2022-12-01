//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include <utility>

using Int = std::int64_t;

auto has_pairs_triplets(std::string const& s) -> std::pair<bool, bool>
{
  std::map<char, Int> counts;
  for (auto c : s) {
    auto it = counts.find(c);
    if (it == counts.end()) {
      counts.insert({c, 1});
    }
    else {
      ++(it->second);
    }
  }

  bool has_pairs{std::any_of(counts.begin(), counts.end(), [](auto x) { return x.second == 2; })};
  bool has_triplets{std::any_of(counts.begin(), counts.end(), [](auto x) { return x.second == 3; })};
  return std::make_pair(has_pairs, has_triplets);
}

auto main() -> int
{
  Int num_pairs{0};
  Int num_triplets{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    auto [has_pair, has_triplet] = has_pairs_triplets(line);
    if (has_pair) { ++ num_pairs; }
    if (has_triplet) { ++ num_triplets;}
  }
  std::cout << "Num pairs: " << num_pairs << '\n';
  std::cout << "Num triplets: " << num_triplets << '\n';
  std::cout << "Result: " << num_pairs * num_triplets << '\n';
  return EXIT_SUCCESS;
}
