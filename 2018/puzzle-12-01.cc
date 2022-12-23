//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <regex>
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
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return EXIT_FAILURE;
  }
  std::smatch m;
  if (!std::regex_match(line, m, initial_re)) {
    std::cerr << "Unable to interpret: " << line << "\n";
    return EXIT_FAILURE;
  }

  std::string state{m.str(1)};
  Int state_offset{0};
  state.insert(state.begin(), 3, soil);
  state.insert(state.end(), 3, soil);
  state_offset -= 3;

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
    std::cout << line << "\n";
    if (m.str(2) == std::string(1, plant)) {
      transitions.at(encode(m.str(1))) = plant;
    }
  }

  for (unsigned i{0}; i < transitions.size(); ++i) {
    std::cout << ((i & 16) ? plant : soil) << ((i & 8) ? plant : soil) << ((i & 4) ? plant : soil)
              << ((i & 2) ? plant : soil) << ((i & 1) ? plant : soil) << " => " << transitions[i]
              << "\n";
  }
  constexpr UInt num_runs{20};
  for (UInt i{0}; i < num_runs; ++i) {
    std::cout << std::setw(2) << i << ": " << state << "\n";
    state.insert(state.begin(), 2, soil);
    state.insert(state.end(), 2, soil);
    state_offset -= 2;
    std::string next_state(2, soil);
    next_state.reserve(state.size());

    UInt encoding{encode(state.substr(0, 4))};
    for (UInt p{4}; p < state.size(); ++p) {
      encoding <<= 1;
      encoding |= state[p] == plant ? 1 : 0;
      encoding &= 31;
      next_state.push_back(transitions[encoding]);
    }
    next_state.insert(next_state.end(), 2, soil);
    assert(next_state.size() == state.size());

    state = next_state;
  }
  std::cout << std::setw(2) << 20 << ": " << state << "\n";

  Int num_plants{std::count(state.begin(), state.end(), plant)};

  Int score{0};
  Int offset{state_offset};
  for (auto c : state) {
    if (c == plant) {
      score += offset;
    }
    ++offset;
  }
  std::cout << "Score: " << score << "\n";
  return EXIT_SUCCESS;
}
