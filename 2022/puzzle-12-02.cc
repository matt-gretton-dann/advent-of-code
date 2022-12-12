#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

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

  [[nodiscard]] auto x() const { return x_; }
  [[nodiscard]] auto y() const { return y_; }

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
  explicit Map(std::istream& is)
  {
    std::string line;
    while (std::getline(is, line)) {
      auto s = line.find('S');
      if (s != std::string::npos) {
        start_ = Pos{static_cast<Int>(s), static_cast<Int>(rows_.size())};
        line[s] = 'a';
      }
      auto e = line.find('E');
      if (e != std::string::npos) {
        end_ = Pos{static_cast<Int>(e), static_cast<Int>(rows_.size())};
        line[e] = 'z';
      }
      rows_.push_back(line);
    }
  }
  [[nodiscard]] auto start() const -> Pos { return start_; }
  [[nodiscard]] auto end() const -> Pos { return end_; }
  [[nodiscard]] auto width() const -> Int { return static_cast<Int>(rows_[0].length()); }
  [[nodiscard]] auto depth() const -> Int { return static_cast<Int>(rows_.size()); }
  [[nodiscard]] auto height(Pos const& p) const -> Height
  {
    return rows_[p.y()][p.x()];
  }

private:
  std::vector<std::string> rows_;
  Pos start_{0, 0};
  Pos end_{0, 0};
};

struct StateTranstitionManager
{
  explicit StateTranstitionManager(Map map) : map_(std::move(map)) {}

  Map map_;

  [[nodiscard]] auto is_finished(Pos const& pos) const -> bool { return map_.height(pos) == 'a'; }

  template<typename Inserter>
  void generate_children(Pos const& current, Inserter inserter) const
  {
    std::array<Pos, 4> const moves{Pos{0, 1}, Pos{0, -1}, Pos{1, 0}, Pos{-1, 0}};
    for (auto const& move : moves) {
      auto pos{current + move};
      // Check in bounds
      if (pos.x() < 0 || pos.x() > map_.width() || pos.y() < 0 || pos.y() > map_.depth()) {
        continue;
      }
      // check height hasn't changed too far
      if (map_.height(pos) < map_.height(current) - 1) {
        continue;
      }
      inserter(pos, 1);
    }
  }
};

auto main() -> int
{
  Map const map(std::cin);
  Pos const initial_state{map.end()};

  auto [finished_state, finished_cost] =
    dijkstra(initial_state, Int{0}, StateTranstitionManager{map});
  std::cout << "Done with cost " << finished_cost << '\n';
  return 0;
}
