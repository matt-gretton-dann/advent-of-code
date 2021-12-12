//
// Created by Matthew Gretton-Dann on 12/12/2021.
//

#include <cctype>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using Paths = std::multimap<std::string, std::string>;
using Route = std::vector<std::string>;

auto is_upper_case(std::string const& s) -> bool { return std::isupper(s[0]) ? true : false; }

auto is_visitable(Route const& route, Paths const& paths, std::string const& proposed) -> bool
{
  if (is_upper_case(proposed)) {
    return true;
  }

  std::set<std::string> visited;
  bool little_visited_twice{false};
  for (auto r : route) {
    if (!is_upper_case(r)) {
      if (visited.contains(r)) {
        little_visited_twice = true;
        break;
      }
      visited.insert(r);
    }
  }

  if (!little_visited_twice) {
    return true;
  }

  auto route_it = std::find(route.begin(), route.end(), proposed);
  return route_it == route.end();
}

template<typename Fn>
void visit(Route& route, Paths const& paths, Fn visitor)
{
  auto [path_begin, path_end] = paths.equal_range(route.back());
  if (path_begin == path_end) {
    visitor(route, paths);
    return;
  }

  for (auto it{path_begin}; it != path_end; ++it) {
    if (is_visitable(route, paths, it->second)) {
      route.push_back(it->second);
      visit(route, paths, visitor);
      route.pop_back();
    }
  }
}

auto main() -> int
{
  Paths paths;
  std::string line;
  while (std::getline(std::cin, line)) {
    auto sep{line.find('-')};
    auto from{line.substr(0, sep)};
    auto to{line.substr(sep + 1)};

    // Make sure we don't visit start or end more than once.
    if (to == "start") {
      std::swap(from, to);
    }
    if (from == "end") {
      std::swap(from, to);
    }
    paths.insert({from, to});
    if (from != "start" && to != "end") {
      paths.insert({to, from});
    }
  }

  unsigned route_count{0};
  Route route{"start"};
  visit(route, paths, [&route_count](auto const& route, auto const&) {
    for (auto r : route) {
      std::cout << " " << r;
    }
    std::cout << "\n";
    ++route_count;
  });
  std::cout << "Routes count: " << route_count;
  return 0;
}
