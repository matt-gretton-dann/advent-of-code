#include <iostream>
#include <regex>
#include <string>

template<typename T>
constexpr auto triangle_sum(T t) noexcept -> T
{
  return t * (t + 1) / 2;
}

auto main() -> int
{
  static std::regex re{R"(target area: x=(-?\d+)\.\.(-?\d+), y=(-?\d+)\.\.(-?\d+))"};
  std::smatch m;
  std::string line;

  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line";
    return 1;
  }
  if (!std::regex_search(line, m, re)) {
    std::cerr << "Unable to interpret: " << line << '\n';
    return 1;
  }

  auto x0{std::stol(m.str(1))};
  auto x1{std::stol(m.str(2))};
  auto y0{std::stol(m.str(3))};
  auto y1{std::stol(m.str(4))};

  auto x_max{std::max(std::abs(x0), std::abs(x1))};
  auto [y_min, y_max] = std::minmax(std::abs(y0), std::abs(y1));
  /* We want to go at most x_max away.  To do this in the most steps we want to be going vertical
   * at or before the last step.  So vx0 must be such that:
   *   vx0 + (vx0 - 1) + ... + 1 + 0 ... 0 <= x_max. (Look triangular number we know a formula...)
   *   => vx0 * (vx0 + 1) / 2 <= x_max
   *
   * I'm lazy - the input is small enough that I can find this by iteration...
   */
  long vx0{1};
  while (triangle_sum(vx0) < x_max) {
    ++vx0;
  }
  /* And correct vx0 here. */
  --vx0;

  std::cout << "vx0: " << vx0 << '\n';

  /* Going the vertically is harder.  Let's do it by iteration.  We know the total number of steps
   * we want to take is at least vx0.
   *
   * Actually - we pass y=0 on the way down, with velocity -vy0.  So we want:
   * triangle_sum(vy_end) - triangle_sum(vy0) <= y_max;
   */
  long candidate_vy0{1};
  long vy0{1};
  while (candidate_vy0 <= y_max) {
    long ypos{0};
    long vy{candidate_vy0};
    while (ypos < y_max) {
      ypos += (vy++);
    }
    ypos -= (--vy);
    if (ypos >= y_min) {
      /* This speed works - let's use update the candidate.  */
      vy0 = candidate_vy0;
    }
    ++candidate_vy0;
  }

  std::cout << "vy0: " << vy0 << '\n';
  std::cout << "Max height:" << triangle_sum(vy0) << '\n';

  return 0;
}
