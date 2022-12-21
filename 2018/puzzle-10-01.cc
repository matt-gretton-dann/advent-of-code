//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <utility>

using Int = std::int64_t;
using Point = std::pair<Int, Int>;

struct Star
{
  auto time_step() noexcept
  {
    pos_.first += velocity_.first;
    pos_.second += velocity_.second;
  }
  Point pos_;
  Point velocity_;
};

using Stars = std::vector<Star>;

auto vert_dist(Stars const& stars)
{
  auto [min, max] =
    std::minmax_element(stars.begin(), stars.end(),
                        [](auto const& l, auto const& r) { return l.pos_.second < r.pos_.second; });
  return max->pos_.second - min->pos_.second;
}

auto main() -> int
{
  std::string line;
  std::regex const re{R"(position=<\s*(-?\d+),\s*(-?\d+)> velocity=<\s*(-?\d+),\s*(-?\d+)>)"};

  Stars stars;
  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_match(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }

    Star star;
    star.pos_.first = std::stoll(m.str(1));
    star.pos_.second = std::stoll(m.str(2));
    star.velocity_.first = std::stoll(m.str(3));
    star.velocity_.second = std::stoll(m.str(4));
    stars.push_back(star);
  }

  auto range{vert_dist(stars)};
  auto old_range{range + 1};
  Stars old_stars;
  Int time{-1};

  // We continue until the vertical range starts getting bigger.
  while (range < old_range) {
    old_range = range;
    old_stars = stars;
    for (auto& star : stars) {
      star.time_step();
    }
    range = vert_dist(stars);
    ++time;
  }

  // We want to use the smallest star set.
  stars = old_stars;
  auto [min_x_elt, max_x_elt] =
    std::minmax_element(stars.begin(), stars.end(),
                        [](auto const& l, auto const& r) { return l.pos_.first < r.pos_.first; });
  auto [min_y_elt, max_y_elt] =
    std::minmax_element(stars.begin(), stars.end(),
                        [](auto const& l, auto const& r) { return l.pos_.second < r.pos_.second; });
  auto min_x{min_x_elt->pos_.first};
  auto max_x{max_x_elt->pos_.first};
  auto min_y{min_y_elt->pos_.second};
  auto max_y{max_y_elt->pos_.second};
  std::vector<std::string> strings(max_y - min_y + 1);
  for (auto& s : strings) {
    s.resize(max_x - min_x + 1, ' ');
  }
  for (auto const& star : stars) {
    strings.at(star.pos_.second - min_y).at(star.pos_.first - min_x) = '#';
  }
  for (auto const& s : strings) {
    std::cout << s << "\n";
  }
  std::cout <<"In " << time << " seconds.\n";

  return EXIT_SUCCESS;
}
