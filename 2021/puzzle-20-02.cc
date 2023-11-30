#include <cassert>
#include <iostream>
#include <string>
#include <vector>

struct State
{
  explicit State(std::string const& rules)
    : rules_(rules)
  {
    assert(rules.size() == 512);
    assert(rules[0] == '.' || rules[511] == '.');
  }

  void add_row(std::string const& row)
  {
    using namespace std::string_literals;
    lights_.push_back(row);
  }

  auto size() const noexcept -> std::size_t
  {
    std::size_t result{0};
    for (auto const& row : lights_) {
      result += std::count(row.begin(), row.end(), '#');
    }
    return result;
  }

  auto next() const -> State
  {
    std::ptrdiff_t longest_line(
      std::max_element(lights_.begin(), lights_.end(), [](auto const& l, auto const& r) {
        return l.size() < r.size();
      })->size());
    State result{rules_};
    if (rules_[0] == '#') {
      result.outside_set_ = !outside_set_;
    }
    std::ptrdiff_t row_count(lights_.size());
    for (std::ptrdiff_t y{-1}; y <= row_count; ++y) {
      std::string next_row;
      next_row.reserve(longest_line);
      for (std::ptrdiff_t x{-1}; x <= longest_line; ++x) {
        next_row.push_back(lit(x, y) ? '#' : '.');
      }
      result.add_row(next_row);
    }
    return result;
  }

  [[nodiscard]] auto lit(std::ptrdiff_t x0, std::ptrdiff_t y0) const noexcept -> bool
  {
    auto num{0};
    for (auto y{y0 - 1}; y < y0 + 2; ++y) {
      for (auto x{x0 - 1}; x < x0 + 2; ++x) {
        num <<= 1;
        num |= bit_set(x, y);
      }
    }
    return rules_[num] == '#';
  }

  void print(std::ostream& os) const
  {
    for (auto const& row : lights_) {
      os << row << '\n';
    }
  }

private:
  auto bit_set(std::ptrdiff_t x, std::ptrdiff_t y) const noexcept -> bool
  {
    if (y < 0 || y >= lights_.size()) {
      return outside_set_;
    }
    if (x < 0 || x >= lights_[y].size()) {
      return outside_set_;
    }
    return lights_[y][x] == '#';
  }

  std::string rules_;
  std::vector<std::string> lights_;
  bool outside_set_{false};
};

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read rules.\n";
    return 1;
  }
  State state{line};
  std::getline(std::cin, line);
  assert(line.empty());
  while (std::getline(std::cin, line) && !line.empty()) {
    state.add_row(line);
  }

  constexpr unsigned count{50};
  for (unsigned i{0}; i < count; ++i) {
    state = state.next();
  }

  std::cout << "Number lit: " << state.size() << '\n';
}
