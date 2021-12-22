#include <array>
#include <iostream>
#include <regex>
#include <string>

using Int = long;
struct Pos
{
  constexpr Pos(Int x, Int y, Int z) : x_(x), y_(y), z_(z) {}
  [[nodiscard]] constexpr auto x() const noexcept -> Int { return x_; }
  [[nodiscard]] constexpr auto y() const noexcept -> Int { return y_; }
  [[nodiscard]] constexpr auto z() const noexcept -> Int { return z_; }

private:
  Int x_{0};
  Int y_{0};
  Int z_{0};
};

constexpr auto operator==(Pos const& lhs, Pos const& rhs) -> bool
{
  return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
}

constexpr auto operator<(Pos const& lhs, Pos const& rhs) -> bool
{
  return lhs.x() < rhs.x() ||
         (lhs.x() == rhs.x() && (lhs.y() < rhs.y() || (lhs.y() == rhs.y() && lhs.z() < rhs.z())));
}

struct State
{
  void set(Pos const& from, Pos const& to) { set(from, to, true); }
  void clear(Pos const& from, Pos const& to) { set(from, to, false); }
  [[nodiscard]] constexpr auto size() const noexcept
  {
    return std::count(nodes_.begin(), nodes_.end(), true);
  }

private:
  constexpr static Int min_coord{-50};
  constexpr static Int max_coord{50};
  constexpr static Int width{max_coord - min_coord + 1};

  void set(Pos const& from, Pos const& to, bool value)
  {
    for (auto x{std::max(min_coord, from.x())}; x <= std::min(max_coord, to.x()); ++x) {
      for (auto y{std::max(min_coord, from.y())}; y <= std::min(max_coord, to.y()); ++y) {
        for (auto z{std::max(min_coord, from.z())}; z <= std::min(max_coord, to.z()); ++z) {
          nodes_[(x - min_coord) * width * width + (y - min_coord) * width + (z - min_coord)] =
            value;
        }
      }
    }
  }

  std::array<bool, (width * width * width)> nodes_{false};
};

auto main() -> int
{
  std::regex re{R"((on|off) x=(-?\d+)..(-?\d+),y=(-?\d+)..(-?\d+),z=(-?\d+)..(-?\d+))"};
  std::string line;
  State state;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cout << "Unable to match: " << line << '\n';
      return 1;
    }

    Pos from{std::stol(m.str(2)), std::stol(m.str(4)), std::stol(m.str(6))};
    Pos to{std::stol(m.str(3)), std::stol(m.str(5)), std::stol(m.str(7))};
    if (m.str(1) == "on") {
      state.set(from, to);
    }
    else {
      state.clear(from, to);
    }
  }

  std::cout << "Number lit: " << state.size() << '\n';
}
