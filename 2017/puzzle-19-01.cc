#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using Map = std::vector<std::string>;

auto main() -> int
{
  std::string line;
  Map map;
  std::size_t len{0};
  while (std::getline(std::cin, line) && !line.empty()) {
    len = std::max(line.size() + 1, len);
    map.push_back(line);
  }
  for (auto& l : map) {
    l.resize(len, ' ');
  }
  map.push_back(std::string(len, ' '));

  int dx{0};
  int dy{1};
  auto x{map[0].find('|')};
  decltype(x) y{0};
  assert(x != std::string::npos);
  std::string letters;

  while (true) {
    switch (map[y][x]) {
    case '|':
    case '-':
      break;
    case ' ':
      std::cout << "Letters: " << letters << '\n';
      return 0;
    case '+':
      if (dx != 0) {
        dx = 0;
        dy = (map[y + 1][x] != ' ') ? 1 : -1;
      }
      else {
        dy = 0;
        dx = (map[y][x + 1] != ' ') ? 1 : -1;
      }
      break;
    default:
      letters += map[y][x];
      break;
    }
    x += dx;
    y += dy;
  }
}
