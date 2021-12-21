#include <array>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using Int = long;
using Coord = std::pair<Int, Int>;

enum class InfectionState { clean, weakened, infected, flagged };

auto progress(InfectionState state) -> InfectionState
{
  switch (state) {
  case InfectionState::clean:
    return InfectionState::weakened;
  case InfectionState::weakened:
    return InfectionState::infected;
  case InfectionState::infected:
    return InfectionState::flagged;
  case InfectionState::flagged:
    return InfectionState::clean;
  }
  abort();
}

struct State
{
  explicit State(std::vector<std::string> const& initial_state)
  {
    Coord pos{0, 0};
    for (auto const& line : initial_state) {
      pos.first = 0;
      for (auto c : line) {
        if (c == '#') {
          infected_.insert({pos, InfectionState::infected});
        }
        ++pos.first;
      }
      ++pos.second;
    }
    pos_ = Coord{pos.first / 2, pos.second / 2};
  }

  void burst()
  {
    auto it{infected_.find(pos_)};
    InfectionState state{it == infected_.end() ? InfectionState::clean : it->second};

    /* Change direction.  */
    switch (state) {
    case InfectionState::clean:
      dir_ = Coord{dir_.second, -dir_.first};
      break;
    case InfectionState::weakened:
      break;
    case InfectionState::infected:
      dir_ = Coord{-dir_.second, dir_.first};
      break;
    case InfectionState::flagged:
      dir_ = Coord{-dir_.first, -dir_.second};
      break;
    }
    /* Update the state and record activity.  */
    state = progress(state);
    if (state == InfectionState::infected) {
      ++activity_;
    }

    /* Insert state into map.  */
    if (it == infected_.end()) {
      infected_.insert({pos_, state});
    }
    else {
      it->second = state;
    }

    /* And move.  */
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
      xmin = std::min(c.first.first, xmin);
      xmax = std::max(c.first.first, xmax);
      ymin = std::min(c.first.second, ymin);
      ymax = std::max(c.first.second, ymax);
    }

    for (auto y{ymin}; y <= ymax; ++y) {
      for (auto x{xmin}; x <= xmax; ++x) {
        Coord pos{x, y};
        auto it{infected_.find(pos)};
        auto state{it == infected_.end() ? InfectionState::clean : it->second};
        std::array<char, 8> cs{'_', 'w', '*', 'f', '.', 'W', '#', 'F'};
        std::cout << cs[static_cast<unsigned>(state) + (pos == pos_ ? 4 : 0)];
      }
      std::cout << '\n';
    }
  }

private:
  std::map<Coord, InfectionState> infected_;
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

  constexpr unsigned count{10'000'000};
  for (unsigned i{0}; i < count; ++i) {
    state.burst();
  }

  std::cout << "Number of new infections: " << state.activity() << '\n';
  return 0;
}
