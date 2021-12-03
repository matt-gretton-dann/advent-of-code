//
// Created by Matthew Gretton-Dann on 03/12/2021.
//

#include <array>
#include <cctype>
#include <iostream>
#include <string>

struct Triangle
{
  explicit Triangle(std::string const& s)
  {
    std::size_t idx{0};
    for (unsigned i = 0; i < 3; ++i) {
      while (idx < s.size() && std::isspace(s[idx]) != 0) {
        ++idx;
      }
      assert(idx < s.size());
      std::size_t end{0};
      lengths_[i] = std::stoul(s.substr(idx), &end);
      idx += end;
    }
  }

  [[nodiscard]] auto is_valid() const -> bool
  {
    return (lengths_[0] + lengths_[1]) > lengths_[2] && (lengths_[0] + lengths_[2]) > lengths_[1] &&
           (lengths_[1] + lengths_[2]) > lengths_[0];
  }

private:
  std::array<std::uint64_t, 3> lengths_;
};

auto main() -> int
{
  unsigned valid{0};
  std::string line;
  while (std::getline(std::cin, line)) {
    Triangle triangle{line};
    if (triangle.is_valid()) {
      ++valid;
    }
  }
  std::cout << "Number of valid triangles: " << valid << '\n';
  return 0;
}