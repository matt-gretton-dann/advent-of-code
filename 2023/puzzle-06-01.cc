#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

using UInt = std::uint64_t;
using NumVec = std::vector<UInt>;

auto operator<<(std::ostream &os, NumVec const &vec) -> std::ostream & {
  for (auto const num: vec) { os << ' ' << num; }
  return os;
}

template<typename It>
void read_nums(It it, std::string_view const nums) {
  char const *pos{nums.data()};
  while (pos - nums.data() < nums.size()) {
    char *next_pos{nullptr};
    *it++ = std::strtoull(pos, &next_pos, 10);
    pos = next_pos;
  }
}

[[nodiscard]] auto time_range(UInt time, UInt distance) -> UInt {
  /* We hold down for `x` units of time and then travel (`t` - `x`) * `x` units of distance.
   * So we want (t - x)x > d => tx - x² > d         (expand)
   *                         => 0 > x² - tx + d     (shuffle)
   *
   * Lets solve the quadratic: x² - tx + d = 0
   * with x = (-b ± √(b² - 4ac))/2a where a = 1, b = -t, c = d
   *   => x = (t ± √(t² - 4d))/2
   *   call two solutions x1 (negative root), x2 (positive root).
   *
   * Answer will the whole number of seconds between x1 and x2.  So:
   *    floor(x2) - ceil(x1) + 1.
   *
   * However if x1 or x2 are the zero point we have to shift them too, so be careful of that.
   */
  double const t{static_cast<double>(time)};
  double const d{static_cast<double>(distance)};
  double const discriminant{std::sqrt(t * t - 4 * d)};
  double const x1{(t - discriminant) / 2};
  double const x2{(t + discriminant) / 2};
  UInt x1i(std::ceil(x1));
  UInt x2i(std::floor(x2));
  if (x1i * x1i - time * x1i + distance <= 0) { ++x1i; }
  if (x2i * x2i - time * x2i + distance <= 0) { --x2i; }
  return x2i - x1i + 1;
}

auto main() -> int try {
  std::string line;

  NumVec times;
  std::getline(std::cin, line);
  assert(line.substr(0, 5) == "Time:");
  read_nums(std::back_inserter(times), line.substr(5));

  NumVec distances;
  std::getline(std::cin, line);
  assert(line.substr(0, 9) == "Distance:");
  read_nums(std::back_inserter(distances), line.substr(9));

  assert(distances.size() == times.size());

  UInt score{1};
  for (std::size_t i{0}; i != times.size(); ++i) {
    auto const tr = time_range(times[i], distances[i]);
    std::cout << times[i] << ' ' << distances[i] << " = " << tr << '\n';
    score *= tr;
  }

  std::cout << "Score: " << score;

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
