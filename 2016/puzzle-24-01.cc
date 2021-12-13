#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using Int = unsigned;
using Coord = std::pair<Int, Int>;
using Map = std::vector<std::string>;
using PointMap = std::map<char, Coord>;
using PointCostMap = std::multimap<char, std::pair<char, Int>>;

void update_cost_map(PointCostMap& cost_map, Map const& map, std::pair<char, Coord> start)
{
  std::set<Coord> visited;
  std::set<Coord> to_visit;
  to_visit.insert(start.second);

  Int cost{0};
  while (!to_visit.empty()) {
    visited.insert(to_visit.begin(), to_visit.end());
    std::set<Coord> next_to_visit;
    for (auto const& n : to_visit) {
      char c{map[n.second][n.first]};
      if (c == '#') {
        abort();
      }

      if (c != '.') {
        cost_map.insert({start.first, {c, cost}});
      }

      auto add = [&map, &visited, &next_to_visit](Int x, Int y) {
        if (map[y][x] != '#' && !visited.contains(Coord{x, y})) {
          next_to_visit.insert(Coord{x, y});
        }
      };
      add(n.first, n.second - 1);
      add(n.first, n.second + 1);
      add(n.first - 1, n.second);
      add(n.first + 1, n.second);
    }
    ++cost;
    std::swap(to_visit, next_to_visit);
  }
}

auto find_lowest_cost(PointCostMap const& cost_map, std::size_t point_count)
  -> std::pair<std::string, Int>
{
  std::multimap<Int, std::string> nodes;
  nodes.insert({0, "0"});

  while (!nodes.empty()) {
    auto it{nodes.begin()};
    Int cost{it->first};
    std::string visited{it->second};
    if (visited.size() == point_count) {
      return {visited, cost};
    }
    nodes.erase(it);

    auto [cm_begin, cm_end] = cost_map.equal_range(visited[visited.size() - 1]);
    for (auto cm_it{cm_begin}; cm_it != cm_end; ++cm_it) {
      if (visited.find(cm_it->second.first) == std::string::npos) {
        nodes.insert({cost + cm_it->second.second, visited + cm_it->second.first});
      }
    }
  }

  abort();
}

auto main() -> int
{
  Map map;
  PointMap points;
  std::string line;

  // Read in map and find the locations we want to visit.
  while (std::getline(std::cin, line) && !line.empty()) {
    auto y{map.size()};
    map.push_back(line);
    std::size_t x{0};
    for (x = line.find_first_not_of(".#", x); x != std::string::npos;
         x = line.find_first_not_of(".#", x + 1)) {
      points.insert({line[x], {x, y}});
    }
  }

  PointCostMap cost_map;
  for (auto const& p : points) {
    update_cost_map(cost_map, map, p);
  }

  auto [route, cost] = find_lowest_cost(cost_map, points.size());
  std::cout << "Route: " << route << " costs " << cost << '\n';
  return 0;
}
