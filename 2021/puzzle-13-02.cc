//
// Created by Matthew Gretton-Dann on 13/12/2021.
//

#include <iostream>
#include <regex>
#include <set>
#include <string>

using Coord = std::pair<long, long>;
using LightSet = std::set<Coord>;

auto fold(LightSet const& lights, Coord const& fold_coord) -> LightSet
{
  LightSet result;
  Coord shift{0, 0};
  for (auto const& c : lights) {
    Coord folded{c.first > fold_coord.first ? (2 * fold_coord.first - c.first) : c.first,
                 c.second > fold_coord.second ? (2 * fold_coord.second - c.second) : c.second};
    shift = {std::min(shift.first, folded.first), std::min(shift.second, folded.second)};
    result.insert(folded);
  }

  if (shift == Coord{0, 0}) {
    return result;
  }

  LightSet result2;
  for (auto const& c : result) {
    result2.emplace(c.first - shift.first, c.second - shift.second);
  }
  return result2;
}

auto print_lights(LightSet lights)
{
  Coord size{0, 0};
  for (auto const& c : lights) {
    size = {std::max(c.first + 1, size.first), std::max(c.second + 1, size.second)};
  }

  std::vector<std::string> display;
  for (long i{0}; i < size.second; ++i) {
    display.push_back(std::string(size.first, ' '));
  }

  for (auto const& c : lights) {
    display[c.second][c.first] = '*';
  }

  for (auto const& l : display) {
    std::cout << l << '\n';
  }
}

auto main() -> int
{
  static std::regex fold_x{"fold along x=(\\d+)"};
  static std::regex fold_y{"fold along y=(\\d+)"};

  std::string line;
  LightSet lights;
  /* Read coords.  */
  while (std::getline(std::cin, line) && !line.empty()) {
    std::size_t end{0};
    auto x{std::stol(line, &end)};
    auto y{std::stol(line.substr(end + 1))};
    lights.emplace(x, y);
  }

  /* Now do folds. */
  while (std::getline(std::cin, line)) {
    std::smatch m;
    Coord fold_coord{std::numeric_limits<long>::max(), std::numeric_limits<long>::max()};
    if (std::regex_search(line, m, fold_x)) {
      fold_coord.first = std::stol(m.str(1));
    }
    else if (std::regex_search(line, m, fold_y)) {
      fold_coord.second = std::stol(m.str(1));
    }
    else {
      std::cerr << "Cannot interpret: " << line << "\n";
      return 1;
    }
    lights = fold(lights, fold_coord);
  }

  print_lights(lights);
  return 0;
}
