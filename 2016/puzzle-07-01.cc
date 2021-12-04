//
// Created by Matthew Gretton-Dann on 04/12/2021.
//

#include <array>
#include <iostream>
#include <string>

auto abba(std::string const& line)
{
  enum State { outside, inside };
  std::array<bool, 2> seen{false, false};
  State state{State::outside};
  for (std::size_t idx{0}; idx < line.size() - 3; ++idx) {
    if (line[idx] == '[') {
      state = State::inside;
    }
    else if (line[idx] == ']') {
      state = State::outside;
    }
    else if (line[idx] != line[idx + 1] && line[idx + 1] == line[idx + 2] &&
             line[idx] == line[idx + 3]) {
      seen[static_cast<unsigned>(state)] = true;
    }
  }
  return seen[State::outside] && !seen[State::inside];
}

auto main() -> int
{
  std::string line;
  unsigned good_count{0};
  while (std::getline(std::cin, line)) {
    if (abba(line)) {
      ++good_count;
    }
  }
  std::cout << "ABBA lines that support TLS: " << good_count << '\n';
  return 0;
}
