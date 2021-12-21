#include <cassert>
#include <iostream>
#include <map>
#include <regex>
#include <string>

[[nodiscard]] auto rotate(std::string const& s) -> std::string
{
  if (s.size() == 4) {
    return std::string{s[2], s[0], s[3], s[1]};
  }
  if (s.size() == 9) {
    return std::string{s[6], s[3], s[0], s[7], s[4], s[1], s[8], s[5], s[2]};
  }
  abort();
}

[[nodiscard]] auto flip(std::string const& s) -> std::string
{
  if (s.size() == 4) {
    return s.substr(2) + s.substr(0, 2);
  }
  if (s.size() == 9) {
    return s.substr(6) + s.substr(3, 3) + s.substr(0, 3);
  }
  abort();
}

struct Patterns
{
  void add(std::string const& s1, std::string const& s2, std::string const& target)
  {
    auto s{s1 + s2};
    for (auto j{0}; j < 2; ++j) {
      for (auto i{0}; i < 4; ++i) {
        map_.insert({s, target});
        s = rotate(s);
      }
      s = flip(s);
    }
  }

  void add(std::string const& s1, std::string const& s2, std::string const& s3,
           std::string const& target)
  {
    auto s{s1 + s2 + s3};
    for (auto j{0}; j < 2; ++j) {
      for (auto i{0}; i < 4; ++i) {
        map_.insert({s, target});
        s = rotate(s);
      }
      s = flip(s);
    }
  }

  std::string const& find(std::string const& s)
  {
    static std::string empty;
    auto it{map_.find(s)};
    if (it == map_.end()) {
      return empty;
    }
    return it->second;
  }

  std::map<std::string, std::string> map_;
};

auto main() -> int
{
  static std::regex re2{"([.#][.#])/([.#][.#]) => ([.#][.#][.#])/([.#][.#][.#])/([.#][.#][.#])"};
  static std::regex re3{
    "([.#][.#][.#])/([.#][.#][.#])/([.#][.#][.#]) => "
    "([.#][.#][.#][.#])/([.#][.#][.#][.#])/([.#][.#][.#][.#])/([.#][.#][.#][.#])"};

  Patterns patterns;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (std::regex_search(line, m, re2)) {
      patterns.add(m.str(1), m.str(2), m.str(3) + m.str(4) + m.str(5));
    }
    else if (std::regex_search(line, m, re3)) {
      patterns.add(m.str(1), m.str(2), m.str(3), m.str(4) + m.str(5) + m.str(6) + m.str(7));
    }
    else {
      std::cerr << "Unable to interpret:" << line << '\n';
      return 1;
    }
  }

  std::string grid{".#...####"};
  unsigned width{3};

  for (unsigned i{0}; i < 5; ++i) {
    assert(grid.size() == width * width);
    std::string new_grid;
    if (width % 2 == 0) {
      for (unsigned y{0}; y < grid.size(); y += width * 2) {
        std::string l1;
        std::string l2;
        std::string l3;
        for (unsigned x{0}; x < width; x += 2) {
          auto pattern{grid.substr(y + x, 2) + grid.substr(y + width + x, 2)};
          auto expand{patterns.find(pattern)};
          assert(!expand.empty());
          l1 += expand.substr(0, 3);
          l2 += expand.substr(3, 3);
          l3 += expand.substr(6);
        }
        new_grid += l1 + l2 + l3;
      }
      width = (width / 2) * 3;
    }
    else {
      assert(width % 3 == 0);
      for (unsigned y{0}; y < grid.size(); y += width * 3) {
        std::string l1;
        std::string l2;
        std::string l3;
        std::string l4;
        for (unsigned x{0}; x < width; x += 3) {
          auto pattern{grid.substr(y + x, 3) + grid.substr(y + width + x, 3) +
                       grid.substr(y + width * 2 + x, 3)};
          auto expand{patterns.find(pattern)};
          assert(!expand.empty());
          l1 += expand.substr(0, 4);
          l2 += expand.substr(4, 4);
          l3 += expand.substr(8, 4);
          l4 += expand.substr(12);
        }
        new_grid += l1 + l2 + l3 + l4;
      }
      width = (width / 3) * 4;
    }

    std::swap(grid, new_grid);
  }

  std::cout << "Number of lit cells: " << std::count(grid.begin(), grid.end(), '#') << '\n';
  return 0;
}
