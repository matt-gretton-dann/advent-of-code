//
// Created by Matthew Gretton-Dann on 03/12/2021.
//

#include <array>
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>

struct Triangle
{
  Triangle() = default;
  Triangle(std::uint64_t l0, std::uint64_t l1, std::uint64_t l2) : lengths_({l0, l1, l2}) {}

  explicit Triangle(std::string const& s)
  {
    std::size_t idx{0};
    for (unsigned i = 0; i < 3; ++i) {
      while (idx < s.size() && std::isspace(s[idx]) != 0) {
        ++idx;
      }
      assert(idx < s.size());
      std::size_t end{0};
      lengths_.at(i) = std::stoul(s.substr(idx), &end);
      idx += end;
    }
  }

  [[nodiscard]] auto is_valid() const -> bool
  {
    return (lengths_[0] + lengths_[1]) > lengths_[2] && (lengths_[0] + lengths_[2]) > lengths_[1] &&
           (lengths_[1] + lengths_[2]) > lengths_[0];
  }

  [[nodiscard]] auto length(unsigned side) const -> std::uint64_t { return lengths_.at(side); }
  void length(unsigned side, std::uint64_t length) { lengths_.at(side) = length; }

private:
  std::array<std::uint64_t, 3> lengths_{0, 0, 0};
};

auto main() -> int
{
  unsigned valid{0};
  std::string line;
  while (true) {
    std::array<Triangle, 3> triangles;
    for (unsigned i = 0; i < 3; ++i) {
      if (!std::getline(std::cin, line)) {
        std::cout << "Number of valid triangles: " << valid << '\n';
        return 0;
      }
      triangles.at(i) = Triangle{line};
    }
    std::array<Triangle, 3> transposed{
      Triangle{triangles[0].length(0), triangles[1].length(0), triangles[2].length(0)},
      Triangle{triangles[0].length(1), triangles[1].length(1), triangles[2].length(1)},
      Triangle{triangles[0].length(2), triangles[1].length(2), triangles[2].length(2)},
    };
    for (unsigned i = 0; i < 3; ++i) {
      if (transposed.at(i).is_valid()) {
        ++valid;
      }
    }
  }
}
