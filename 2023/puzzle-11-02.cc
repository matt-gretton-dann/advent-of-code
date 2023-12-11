#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using UInt = std::uint64_t;
using StringVec = std::vector<std::string>;
using UIntVec = std::vector<UInt>;
using Pos = std::pair<UInt, UInt>;
using PosVec = std::vector<Pos>;

struct StarMap
{
  void push_line(std::string const& line)
  {
    map_.push_back(line);
    if (horiz_stars_.size() < line.size()) {
      horiz_stars_.resize(line.size(), 0);
    }

    UInt row_count{0};
    for (auto pos{line.find('#')}; pos != std::string::npos; pos = line.find('#', pos + 1)) {
      horiz_stars_[pos] += 1;
      stars_.emplace_back(std::make_pair(pos, vert_stars_.size()));
      ++row_count;
    }

    vert_stars_.push_back(row_count);
  }

  [[nodiscard]] auto star_distances(UInt expansion_coefficient) const -> UInt
  {
    UInt size{0};
    for (auto it1{stars_.begin()}; it1 != stars_.end(); ++it1) {
      for (auto it2{it1 + 1}; it2 != stars_.end(); ++it2) {
        size += distance(it1, it2, expansion_coefficient);
      }
    }

    return size;
  }

private:
  [[nodiscard]] auto distance(PosVec::const_iterator star1, PosVec::const_iterator star2,
                              UInt const expansion_coefficient) const -> UInt
  {
    auto [x1, y1] = *star1;
    auto [x2, y2] = *star2;

    if (x1 > x2) { std::swap(x1, x2); }
    if (y1 > y2) { std::swap(y1, y2); }

    return (x2 - x1) + (y2 - y1) + (expansion_coefficient - 1) * (
             horiz_empty(x1, x2) + vert_empty(y1, y2));
  }

  [[nodiscard]] auto horiz_empty(UInt const x1, UInt const x2) const -> UInt
  {
    return std::count_if(horiz_stars_.begin() + static_cast<StringVec::difference_type>(x1),
                         horiz_stars_.begin() + static_cast<StringVec::difference_type>(x2), [](UInt const count) {
                           return count == 0;
                         });
  }

  [[nodiscard]] auto vert_empty(UInt const x1, UInt const x2) const -> UInt
  {
    return std::count_if(vert_stars_.begin() + static_cast<StringVec::difference_type>(x1),
                         vert_stars_.begin() + static_cast<StringVec::difference_type>(x2), [](UInt const count) {
                           return count == 0;
                         });
  }

  StringVec map_;
  UIntVec horiz_stars_;
  UIntVec vert_stars_;
  PosVec stars_;
};

auto main() -> int try {
  std::string line;
  StarMap map;

  while (std::getline(std::cin, line)) {
    map.push_line(line);
  }

  std::cout << "Star map distance (coeff 2): " << map.star_distances(2) << '\n';
  std::cout << "Star map distance (coeff 10): " << map.star_distances(10) << '\n';
  std::cout << "Star map distance (coeff 100): " << map.star_distances(100) << '\n';
  std::cout << "Star map distance (coeff 1000000): " << map.star_distances(1000000) << '\n';

  return EXIT_SUCCESS;;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
