//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <utility>

using Int = std::int64_t;

template<typename iterator>
auto read_int(iterator& begin, iterator end) -> Int
{
  while (begin != end && *begin == ' ') {
    ++begin;
  }
  Int result{0};
  while (begin != end && std::isdigit(*begin) != 0) {
    result *= 10;
    result += *begin - '0';
    ++begin;
  }
  return result;
}

template<typename iterator>
auto process_string(iterator& begin, iterator end) -> Int
{
  Int child_count{read_int(begin, end)};
  Int metadata_count{read_int(begin, end)};
  std::vector<Int> child_scores;
  child_scores.push_back(0);
  for (Int i{0}; i < child_count; ++i) {
    child_scores.push_back(process_string(begin, end));
  }

  Int score{0};
  for (Int i{0}; i < metadata_count; ++i) {
    if (child_count > 0) {
      Int idx{read_int(begin, end)};
      if (idx < child_scores.size()) {
        score += child_scores[idx];
      }
    }
    else {
      score += read_int(begin, end);
    }
  }
  return score;
}

auto main() -> int
{
  std::string line;

  // Read data
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }

  auto begin{line.begin()};
  Int sum{process_string(begin, line.end())};
  std::cout << "Sum: " << sum << "\n";

  return EXIT_SUCCESS;
}
