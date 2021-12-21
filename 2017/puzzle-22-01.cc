#include <iostream>
#include <set>
#include <string>
#include <vector>

using Int = long;
using Coord = std::pair<Int, Int>;

struct State
{
  explicit State(std::vector<std::string> const& initial_state)
  {
    Coord pos{0, 0};
    for (auto const& line : initial_state) {
      pos.first = 0;
      for (auto c : line) {
        if (c == '#') {
          infected_.insert(pos);
        }
        ++pos.first;
      }
      ++pos.second;
    }
    pos_ = Coord{pos.first / 2, pos.second / 2};
  }

  void burst()
  {
    if (infected_.contains(pos_)) {
      dir_ = Coord{-dir_.second, dir_.first};
      infected_.erase(pos_);
    }
    else {
      dir_ = Coord{dir_.second, -dir_.first};
      infected_.insert(pos_);
      ++activity_;
    }
    pos_ = Coord{pos_.first + dir_.first, pos_.second + dir_.second};
  }

  [[nodiscard]] auto activity() const -> std::size_t { return activity_; }

  void print() const
  {
    Int xmin{std::numeric_limits<Int>::max()};
    Int xmax{std::numeric_limits<Int>::min()};
    Int ymin{std::numeric_limits<Int>::max()};
    Int ymax{std::numeric_limits<Int>::min()};
    for (auto const& c : infected_) {
      xmin = std::min(c.first, xmin);
      xmax = std::max(c.first, xmax);
      ymin = std::min(c.second, ymin);
      ymax = std::max(c.second, ymax);
    }

    for (auto y{ymin}; y <= ymax; ++y) {
      for (auto x{xmin}; x <= xmax; ++x) {
        Coord pos{x, y};
        bool infected{infected_.contains(pos)};
        if (pos == pos_) {
          std::cout << (infected ? '*' : '_');
        }
        else {
          std::cout << (infected ? '#' : '.');
        }
      }
      std::cout << '\n';
    }
  }

private:
  std::set<Coord> infected_;
  Coord pos_;
  Coord dir_{0, -1};
  Int activity_{0};
};

auto main() -> int
{
  std::vector<std::string> initial;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    initial.push_back(line);
  }

  State state(initial);

  constexpr unsigned count{10'000};
  for (unsigned i{0}; i < count; ++i) {
    state.burst();
  }

  std::cout << "Number of new infections: " << state.activity() << '\n';
  return 0;
}
