//
// Created by Matthew Gretton-Dann on 04/12/2021.
//

#include <array>
#include <iostream>
#include <string>
#include <vector>

auto aba(std::string const& line) -> bool
{
  enum State { outside, inside };
  using AbaArray = std::array<char, 2>;
  std::vector<AbaArray> abas;
  std::vector<AbaArray> pending_abas;

  State state{outside};
  for (std::size_t idx{0}; idx < line.size() - 2; ++idx) {
    if (line[idx] == '[') {
      state = inside;
    }
    else if (line[idx] == ']') {
      state = outside;
    }
    else if (line[idx] != line[idx + 1] && line[idx] == line[idx + 2]) {
      if (state == outside) {
        char a{line[idx]};
        char b{line[idx + 1]};
        abas.push_back({a, b});
        if (std::find_if(pending_abas.begin(), pending_abas.end(), [a, b](auto const& it) {
              return it[0] == a && it[1] == b;
            }) != pending_abas.end()) {
          return true;
        }
      }
      else {
        char b{line[idx]};
        char a{line[idx + 1]};
        if (std::find_if(abas.begin(), abas.end(), [a, b](auto const& it) {
              return it[0] == a && it[1] == b;
            }) != abas.end()) {
          return true;
        }
        pending_abas.push_back({a, b});
      }
    }
  }

  return false;
}

auto main() -> int
{
  std::string line;
  unsigned good_count{0};
  while (std::getline(std::cin, line)) {
    if (aba(line)) {
      ++good_count;
    }
  }
  std::cout << "ABBA lines that support SSL: " << good_count << '\n';
  return 0;
}
