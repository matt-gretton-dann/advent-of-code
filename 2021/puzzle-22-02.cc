#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

using Int = std::int64_t;
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
  void add(Pos const& from, Pos const& to, bool set)
  {
    x_segments_.push_back(from.x());
    x_segments_.push_back(to.x() + 1);
    y_segments_.push_back(from.y());
    y_segments_.push_back(to.y() + 1);
    z_segments_.push_back(from.z());
    z_segments_.push_back(to.z() + 1);
    instructions_.emplace_back(from, to, set);
  }

  void calculate()
  {
    std::sort(x_segments_.begin(), x_segments_.end());
    x_segments_.erase(std::unique(x_segments_.begin(), x_segments_.end()), x_segments_.end());
    std::sort(y_segments_.begin(), y_segments_.end());
    y_segments_.erase(std::unique(y_segments_.begin(), y_segments_.end()), y_segments_.end());
    std::sort(z_segments_.begin(), z_segments_.end());
    z_segments_.erase(std::unique(z_segments_.begin(), z_segments_.end()), z_segments_.end());
    auto x_size{x_segments_.size()};
    auto y_size{y_segments_.size()};
    auto z_size{z_segments_.size()};

    nodes_.resize(x_size * y_size * z_size, false);
    for (auto const& i : instructions_) {
      auto xit_begin{std::lower_bound(x_segments_.begin(), x_segments_.end(), std::get<0>(i).x())};
      auto yit_begin{std::lower_bound(y_segments_.begin(), y_segments_.end(), std::get<0>(i).y())};
      auto zit_begin{std::lower_bound(z_segments_.begin(), z_segments_.end(), std::get<0>(i).z())};
      auto xit_end{
        std::lower_bound(x_segments_.begin(), x_segments_.end(), std::get<1>(i).x() + 1)};
      auto yit_end{
        std::lower_bound(y_segments_.begin(), y_segments_.end(), std::get<1>(i).y() + 1)};
      auto zit_end{
        std::lower_bound(z_segments_.begin(), z_segments_.end(), std::get<1>(i).z() + 1)};
      for (auto zit{zit_begin}; zit != zit_end; ++zit) {
        for (auto yit{yit_begin}; yit != yit_end; ++yit) {
          for (auto xit{xit_begin}; xit != xit_end; ++xit) {
            auto x{std::distance(x_segments_.begin(), xit)};
            auto y{std::distance(y_segments_.begin(), yit)};
            auto z{std::distance(z_segments_.begin(), zit)};
            nodes_[z * y_size * x_size + y * x_size + x] = std::get<2>(i);
          }
        }
      }
    }
  }

  [[nodiscard]] constexpr auto size() const noexcept -> Int
  {
    Int size{0};
    auto x_size{x_segments_.size()};
    auto y_size{y_segments_.size()};

    for (auto zit{z_segments_.begin()}; zit + 1 != z_segments_.end(); ++zit) {
      for (auto yit{y_segments_.begin()}; yit + 1 != y_segments_.end(); ++yit) {
        for (auto xit{x_segments_.begin()}; xit + 1 != x_segments_.end(); ++xit) {
          auto x{std::distance(x_segments_.begin(), xit)};
          auto y{std::distance(y_segments_.begin(), yit)};
          auto z{std::distance(z_segments_.begin(), zit)};
          if (nodes_[z * y_size * x_size + y * x_size + x]) {
            size += (xit[1] - xit[0]) * (yit[1] - yit[0]) * (zit[1] - zit[0]);
          }
        }
      }
    }
    return size;
  }

private:
  std::vector<std::tuple<Pos, Pos, bool>> instructions_;
  std::vector<Int> x_segments_;
  std::vector<Int> y_segments_;
  std::vector<Int> z_segments_;
  std::vector<bool> nodes_;
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

    Pos from(std::stol(m.str(2)), std::stol(m.str(4)), std::stol(m.str(6)));
    Pos to(std::stol(m.str(3)), std::stol(m.str(5)), std::stol(m.str(7)));
    state.add(from, to, m.str(1) == "on");
  }

  state.calculate();
  std::cout << "Number lit: " << state.size() << '\n';
}
