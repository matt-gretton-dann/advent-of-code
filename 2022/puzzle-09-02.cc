//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <utility>

using Int = std::int64_t;
using Pos = std::pair<Int, Int>;

struct compare_pos
{
  constexpr auto operator()(Pos const& l, Pos const& r) const noexcept
  {
    if (l.second < r.second) {
      return true;
    }
    if (l.second == r.second && l.first < r.first) {
      return true;
    }
    return false;
  }
};

struct State
{
  State()
  {
    for (auto it{rope_.begin()}; it != rope_.end(); ++it) {
      *it = {0, 0};
    }
    visited_.insert({0, 0});
  }

  auto move(Pos dir, Int steps)
  {
    for (Int i{0}; i != steps; ++i) {
      auto it = rope_.begin();
      it->first += dir.first;
      it->second += dir.second;

      Pos pos{*it};
      while (it != rope_.end()) {
        if (std::abs(it->first - pos.first) > 1 || std::abs(it->second - pos.second) > 1) {
          if (it->first < pos.first) {
            ++it->first;
          }
          if (it->first > pos.first) {
            --it->first;
          }
          if (it->second < pos.second) {
            ++it->second;
          }
          if (it->second > pos.second) {
            --it->second;
          }
        }
        pos = *it;
        ++it;
      }

      visited_.insert(pos);
    }
  }

  [[nodiscard]] auto visited_count() const noexcept { return visited_.size(); }

  std::array<Pos, 10> rope_;
  std::set<Pos, compare_pos> visited_;
};

auto main() -> int
{
  std::string line;
  State state;

  while (std::getline(std::cin, line) && !line.empty()) {
    auto item{line[0]};
    Pos dir{0, 0};
    Int const amount{std::stoll(line.substr(2))};
    switch (item) {
    case 'L':
      dir.first = -1;
      break;
    case 'R':
      dir.first = 1;
      break;
    case 'U':
      dir.second = 1;
      break;
    case 'D':
      dir.second = -1;
      break;
    default:
      std::cerr << "Unable to interpret: " << line << '\n';
      return EXIT_FAILURE;
    }
    state.move(dir, amount);
  }

  std::cout << "Number of locations visited: " << state.visited_count() << '\n';
  return 0;
}
