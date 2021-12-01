#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using Coord = long;
using Position = std::pair<Coord, Coord>;
using Tiles = std::set<Position>;

struct Tiler {
  void flip_tile(std::string const &s) {
    Coord x = 0;
    Coord y = 0;
    auto it = s.begin();
    while (it != s.end()) {
      Coord dx = 2;
      if (*it == 's' || *it == 'n') {
        y += (*it == 'n') - (*it == 's');
        ++it;
        dx = 1;
        assert(it != s.end());
      }

      if (*it == 'e') {
        x += dx;
      } else if (*it == 'w') {
        x -= dx;
      } else {
        assert(false);
      }
      ++it;
    }

    // Insert tile to be flipped - if we fail remove it.
    auto [iit, success] = black_tiles_.insert({x, y});
    if (!success) {
      black_tiles_.erase(iit);
    }
  }

  std::size_t black_count() const noexcept { return black_tiles_.size(); }

private:
  Tiles black_tiles_;
};

int main(void) {
  Tiler tiler;
  std::string line;
  while (std::getline(std::cin, line)) {
    tiler.flip_tile(line);
  }

  std::cout << "Number of black tiles: " << tiler.black_count() << "\n";

  return 0;
}