//
// Created by Matthew Gretton-Dann on 11/12/2021.
//

#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using Coord = std::pair<int, int>;

struct State
{
  State(std::vector<std::string> const& initial)
  {
    for (int y = 0; y < initial.size(); ++y) {
      for (int x = 0; x < initial[y].size(); ++x) {
        octopi_.insert({{x, y}, initial[y][x] - '0'});
      }
    }
  }

  unsigned sparkle()
  {
    std::set<Coord> visited;
    // Start by incrementing every octopus by one.
    for (auto& o : octopi_) {
      (o.second)++;
    }

    auto just_visited{0};
    do {
      just_visited = visited.size();
      for (auto const& o : octopi_) {
        if (o.second > 9 && !visited.contains(o.first)) {
          visited.insert({o.first});
          for (int x = o.first.first - 1; x < o.first.first + 2; ++x) {
            for (int y = o.first.second - 1; y < o.first.second + 2; ++y) {
              auto it{octopi_.find({x, y})};
              if (it != octopi_.end()) {
                ++(it->second);
              }
            }
          }
        }
      }
    } while (just_visited != visited.size());

    // And reset the energy levels
    for (auto& o : octopi_) {
      if (o.second > 9) {
        o.second = 0;
      }
    }

    return visited.size();
  }

private:
  std::map<Coord, unsigned> octopi_;
};

auto main() -> int
{
  std::string line;
  std::vector<std::string> lines;

  while (std::getline(std::cin, line)) {
    lines.push_back(line);
  }

  State octopi(lines);

  constexpr unsigned max_steps{100};
  unsigned count{0};
  for (unsigned i = 0; i < max_steps; ++i) {
    count += octopi.sparkle();
  }

  std::cout << "After " << max_steps << " we have sparkled " << count << " times.\n";
  return 0;
}
