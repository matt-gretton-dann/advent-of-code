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

  auto [x_min, x_max] = std::minmax(std::abs(x0), std::abs(x1));
  auto [y_min, y_max] = std::minmax(std::abs(y0), std::abs(y1));

  /* Find a minimum vx0.  */
  long vx0_min{1};
  while (triangle_sum(vx0_min) < x_min) {
    ++vx0_min;
  }

  unsigned count_success{0};

  /* For each of the possible starting velocities.  */
  for (long vy0_start{-y_max}; vy0_start <= y_max; ++vy0_start) {
    for (long vx0_start{vx0_min}; vx0_start <= x_max; ++vx0_start) {
      auto vx0{vx0_start};
      auto vy0{vy0_start};
      long x{0};
      long y{0};

      /* Iterate until we're in the target range in the x direction.  We know this will happen as
       * vx0_start is at least vx0_min.
       */
      while (x < x_min) {
        x += vx0;
        vx0 = std::max(vx0 - 1, long{0});
        y += (vy0--);
      }

      /* Now check to see where whether the y co-ordinate will end up in the target zone whilst
       * we're still in the zone horizontally.
       */
      while (x <= x_max && -y <= y_max) {
        if (y_min <= -y && -y <= y_max) {
          ++count_success;
          break;
        }
        x += vx0;
        vx0 = std::max(vx0 - 1, long{0});
        y += (vy0--);
      }
    }
  }
  
  std::cout << "Count of successful trajectories:" << count_success << '\n';

  return 0;
}
