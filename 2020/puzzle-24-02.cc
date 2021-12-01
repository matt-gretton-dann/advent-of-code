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
struct State {
  bool black_ = false;
  int neighbours_ = 0;
};
using Tiles = std::map<Position, State>;

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
    flip_tile(x, y);
  }

  Tiler next_state() const {
    Tiler next(*this);
    for (auto const &kv : tiles_) {
      if (kv.second.black_ &&
          (kv.second.neighbours_ == 0 || kv.second.neighbours_ > 2)) {
        next.flip_tile(kv.first.first, kv.first.second);
      } else if (!kv.second.black_ && kv.second.neighbours_ == 2) {
        next.flip_tile(kv.first.first, kv.first.second);
      }
    }

    return next;
  }

  std::size_t black_count() const noexcept {
    std::size_t count = 0;
    for (auto const &kv : tiles_) {
      count += kv.second.black_;
    }

    return count;
  }

private:
  void set_neighbours(Coord x, Coord y, int delta) {
    set_neighbour(x + 2, y, delta);
    set_neighbour(x + 1, y - 1, delta);
    set_neighbour(x - 1, y - 1, delta);
    set_neighbour(x - 2, y, delta);
    set_neighbour(x - 1, y + 1, delta);
    set_neighbour(x + 1, y + 1, delta);
  }

  void set_neighbour(Coord x, Coord y, int delta) {
    auto [it, success] = tiles_.insert({{x, y}, {false, delta}});
    if (!success) {
      it->second.neighbours_ += delta;
    }
    assert(it->second.neighbours_ >= 0);
  }

  void flip_tile(Coord x, Coord y) {
    auto [it, success] = tiles_.insert({{x, y}, {true, 0}});
    if (!success) {
      it->second.black_ = !it->second.black_;
    }
    if (it->second.black_) {
      set_neighbours(x, y, 1);
    } else {
      set_neighbours(x, y, -1);
    }
  }

private:
  Tiles tiles_;
};

int main(void) {
  Tiler tiler;
  std::string line;
  while (std::getline(std::cin, line)) {
    tiler.flip_tile(line);
  }

  std::cout << "Day 0: " << tiler.black_count() << "\n";
  for (unsigned i = 1; i < 101; ++i) {
    tiler = tiler.next_state();
    std::cout << "Day " << i << ": " << tiler.black_count() << "\n";
  }

  return 0;
}