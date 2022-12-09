//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

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
  State() { visited_.insert(tail_pos_);
  }

  auto move(Pos dir, Int steps)
  {
    for (Int i{0}; i != steps; ++i) {
      head_pos_.first += dir.first;
      head_pos_.second += dir.second;

      if (std::abs(head_pos_.first - tail_pos_.first) > 1 ||
          std::abs(head_pos_.second - tail_pos_.second) > 1) {
        if (tail_pos_.first < head_pos_.first) {
          ++tail_pos_.first;
        }
        if (tail_pos_.first > head_pos_.first) {
          --tail_pos_.first;
        }
        if (tail_pos_.second < head_pos_.second) {
          ++tail_pos_.second;
        }
        if (tail_pos_.second > head_pos_.second) {
          --tail_pos_.second;
        }
        visited_.insert(tail_pos_);
      }
    }
  }

  [[nodiscard]] auto visited_count() const noexcept { return visited_.size(); }

  Pos head_pos_{0, 0};
  Pos tail_pos_{0, 0};
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
