//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cassert>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <utility>

using Int = std::int64_t;
using UInt = std::uint64_t;

constexpr char plant{'#'};
constexpr char soil{'.'};

unsigned encode(std::string const& s)
{
  UInt result{0};
  for (auto c : s) {
    result <<= 1;
    result |= (c == plant) ? 1 : 0;
  }
  return result;
}

auto main() -> int
{
  std::string line;
  std::regex const initial_re{"^initial state: ([#.]+)$"};
  std::regex const map_re{"^([#.]{5}) => ([#.])"};
  std::vector<Int> state;

  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }
  std::smatch m;
  if (!std::regex_match(line, m, initial_re)) {
    std::cerr << "Unable to interpret: " << line << "\n";
    return EXIT_FAILURE;
  }

  Int offset{0};
  for (auto c : m.str(1)) {
    if (c == plant) {
      state.push_back(offset);
    }
    ++offset;
  }

  std::vector<char> transitions(32, soil);
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }

    std::smatch m;
    if (!std::regex_match(line, m, map_re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }

    if (m.str(2) == std::string(1, plant)) {
      transitions.at(encode(m.str(1))) = plant;
    }
  }

  constexpr UInt num_runs{50'000'000'000};
  for (UInt i{0}; i < num_runs; ++i) {
    if (i % 100'000'000 == 0) {
      Int score{
        std::accumulate(state.begin(), state.end(), Int{0}, [](Int a, Int c) { return a + c; })};
      std::cout << std::setw(13) << i << ": " << score << "\n";
    }
    Int offset_begin{state.front()};
    Int offset_end{state.back() + 4};

    std::vector<Int> next_state;
    next_state.reserve(state.size());
    UInt encoding{0};
    auto it{state.begin()};
    while (offset_begin != offset_end) {
      encoding <<= 1;
      assert(it == state.end() || *it <= offset_begin);
      if (it != state.end() && *it == offset_begin) {
        encoding |= 1;
        ++it;
      }
      encoding &= 31;

      if (transitions.at(encoding) == plant) {
        next_state.push_back(offset_begin - 2);
      }
      ++offset_begin;
    }

    state = next_state;
  }

  Int score{
    std::accumulate(state.begin(), state.end(), Int{0}, [](Int a, Int c) { return a + c; })};
  std::cout << "Score: " << score << "\n";
  return EXIT_SUCCESS;
}
