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

struct ConwayCubeState
{
  using Point = std::tuple<int, int, int>;
  using PointSet = std::set<Point>;
  using NeighbourMap = std::map<Point, unsigned>;

  ConwayCubeState() : next_row_(0) {}
  void add_row(std::string const& s)
  {
    for (int x = 0; x < s.size(); ++x) {
      if (s[x] == '#') {
        set_point(x, next_row_, 0);
      }
    }
    ++next_row_;
  }

  ConwayCubeState next_state() const
  {
    ConwayCubeState next;
    for (auto const& n : neighbours_) {
      bool active = points_.find(n.first) != points_.end();
      if (active && (n.second == 2 || n.second == 3)) {
        next.set_point(n.first);
      }
      else if (!active && n.second == 3) {
        next.set_point(n.first);
      }
    }

    return next;
  }

  auto active() const { return points_.size(); }

private:
  void set_point(Point const& pt) { set_point(std::get<0>(pt), std::get<1>(pt), std::get<2>(pt)); }

  void set_point(int px, int py, int pz)
  {
    points_.insert({px, py, pz});

    for (int x = px - 1; x < px + 2; ++x) {
      for (int y = py - 1; y < py + 2; ++y) {
        for (int z = pz - 1; z < pz + 2; ++z) {
          if (x == px && y == py && z == pz) {
            continue;
          }
          auto [it, success] = neighbours_.insert({{x, y, z}, 1});
          if (!success) {
            assert(it != neighbours_.end());
            it->second += 1;
          }
        }
      }
    }
  }

  int next_row_;
  PointSet points_;
  NeighbourMap neighbours_;
};

int main(void)
{
  std::string line;
  ConwayCubeState state;
  while (std::getline(std::cin, line)) {
    state.add_row(line);
    std::cout << line << "\n";
  }

  std::cout << "Initial active count = " << state.active() << "\n";

  for (unsigned i = 0; i < 6; ++i) {
    state = state.next_state();
    std::cout << i + 1 << ": active count = " << state.active() << "\n";
  }

  return 0;
}