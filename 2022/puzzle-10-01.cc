//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <numeric>
#include <string>
#include <utility>

using Int = std::int64_t;
using UInt = std::uint64_t;

struct State
{
  auto addx(UInt incr, Int delta)
  {
    pc_ += incr;
    if (current_sample_ != sample_points_.end() && *current_sample_ < pc_) {
      std::cout << "Sample point: " << *current_sample_ << " value: " << x_ << " score "
                << x_ * *current_sample_ << " total " << score_ << '\n';
      score_ += x_ * *current_sample_;
      ++current_sample_;
    }
    x_ += delta;
  }

  [[nodiscard]] auto score() const noexcept -> Int { return score_; }

  Int x_{1};
  Int score_{0};
  UInt pc_{1};
  static std::array<UInt, 6> const sample_points_;
  std::array<UInt, 6>::const_iterator current_sample_{sample_points_.begin()};
};

std::array<UInt, 6> const State::sample_points_ = {20, 60, 100, 140, 180, 220};

auto main() -> int
{
  std::string line;
  State state;
  using namespace std::literals::string_literals;

  while (std::getline(std::cin, line) && !line.empty()) {
    if (line == "noop"s) {
      state.addx(1, 0);
    }
    else if (line.substr(0, 5) == "addx "s) {
      state.addx(2, std::stoll(line.substr(5)));
    }
    else {
      std::cerr << "Unable to interpret: " << line << '\n';
      return EXIT_FAILURE;
    }
  }
  std::cout << "Score: " << state.score() << '\n';
  return 0;
}
