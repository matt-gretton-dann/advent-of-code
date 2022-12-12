#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <set>
#include <string>

#include "graph-utils.h"

using Int = std::int64_t;

struct Pos
{
  Pos() noexcept = default;
  Pos(Int x, Int y) noexcept : x_(x), y_(y) {}
  Pos(Pos const&) noexcept = default;
  auto operator=(Pos const&) noexcept -> Pos& = default;
  Pos(Pos&&) noexcept = default;
  auto operator=(Pos&&) noexcept -> Pos& = default;
  ~Pos() noexcept = default;

  auto x() const { return x_; }
  auto y() const { return y_; }

private:
  Int x_{0};
  Int y_{0};
};

auto operator+(Pos const& x, Pos const& y) noexcept -> Pos
{
  return Pos{x.x() + y.x(), x.y() + y.y()};
}

auto operator<(Pos const& l, Pos const& r) noexcept -> bool
{
  return l.y() < r.y() || (l.y() == r.y() && l.x() < r.x());
}

auto operator==(Pos const& l, Pos const& r) noexcept -> bool
{
  return l.x() == r.x() && l.y() == r.y();
}

using Height = char;

struct Map
{
  Map(std::istream& is)
  {
    std::string line;
    while (std::getline(std::cin, line)) {
      auto s = line.find('S');
      if (s != std::string::npos) {
        start_ = Pos{static_cast<Int>(s), static_cast<Int>(rows_.size())};
      }
      auto e = line.find('E');
      if (e != std::string::npos) {
        end_ = Pos{static_cast<Int>(e), static_cast<Int>(rows_.size())};
      }
      rows_.push_back(line);
    }
  }
  auto start() const -> Pos { return start_; }
  auto end() const -> Pos { return end_; }
  auto width() const -> Int { return rows_[0].length(); }
  auto depth() const -> Int { return rows_.size(); }
  auto height(Pos const& p) const -> Height
  {
    if (p == start_) {
      return 'a';
    }
    if (p == end_) {
      return 'z';
    }
    return rows_[p.y()][p.x()];
  }

private:
  std::vector<std::string> rows_;
  Pos start_{0, 0};
  Pos end_{0, 0};
};

struct StateTranstitionManager
{
  explicit StateTranstitionManager(Map const& map) : map_(map) {}

  Map const& map_;

  bool is_finished(Pos const& pos) { return pos == map_.end(); }

  template<typename Inserter>
  void generate_children(Pos const& current, Inserter inserter)
  {
    std::array<Pos, 4> moves = {Pos{0, 1}, Pos{0, -1}, Pos{1, 0}, Pos{-1, 0}};
    for (auto const& move : moves) {
      auto pos{current + move};
      // Check in bounds
      if (pos.x() < 0 || pos.x() > map_.width() || pos.y() < 0 || pos.y() > map_.depth()) {
        continue;
      }
      // check height hasn't changed too far
      if (map_.height(pos) > map_.height(current) + 1) {
        continue;
      }
      inserter(pos, 1);
    }
  }
};

auto main() -> int
{
  Map map(std::cin);
  Pos initial_state{map.start()};

  auto [finished_state, finished_cost] =
    dijkstra(initial_state, Int{0}, StateTranstitionManager{map});
  std::cout << "Done with cost " << finished_cost << '\n';
  return 0;
}
