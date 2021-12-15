#include <iostream>
#include <string>

using Coord = std::pair<int, int>;

auto move(Coord const& c, std::string const& dir) -> Coord
{
  if (dir == "nw") {
    return {c.first - 1, c.second + 1};
  }
  if (dir == "n") {
    return {c.first, c.second + 2};
  }
  if (dir == "ne") {
    return {c.first + 1, c.second + 1};
  }
  if (dir == "se") {
    return {c.first + 1, c.second - 1};
  }
  if (dir == "s") {
    return {c.first, c.second - 2};
  }
  if (dir == "sw") {
    return {c.first - 1, c.second - 1};
  }
  abort();
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line\n";
    return 1;
  }

  std::size_t idx{0};
  Coord pos{0, 0};
  while (idx < line.size()) {
    auto next{line.find(',', idx)};
    if (next == std::string::npos) {
      next = line.size();
    }
    pos = move(pos, line.substr(idx, next - idx));

    idx = next + 1;
  }

  std::cout << "Ended up at (" << pos.first << ", " << pos.second << ") moves away.\n";
  /* We move horizontally pos.first steps - which gets us up to pos.first steps vertically as well.
   * Then we move the remaining vertical distances.  */
  auto x_dist{std::abs(pos.first)};
  auto y_dist{std::abs(pos.second)};
  auto moves{x_dist};
  if (y_dist > x_dist) {
    moves += (y_dist - x_dist) / 2;
  }
  std::cout << "Ended up " << moves << " moves away.\n";
  return 0;
}
