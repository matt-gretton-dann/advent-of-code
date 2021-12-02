#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

struct Box
{
  /** Construct box.
   *  \param s String representation of dimensions 'lxwxh'
   */
  explicit Box(std::string const& s)
  {
    std::size_t pos = 0;
    l_ = std::stoul(s, &pos, 10);
    assert(s[pos] == 'x');
    auto s2 = s.substr(pos + 1);
    w_ = std::stoul(s2, &pos, 10);
    assert(s2[pos] == 'x');
    s2 = s2.substr(pos + 1);
    h_ = std::stoul(s2, &pos, 10);
    assert(pos == s2.length());
  }

  // How much paper does this box need?
  [[nodiscard]] auto paper_needed() const -> std::uint64_t
  {
    std::uint64_t s1 = l_ * w_;
    std::uint64_t s2 = w_ * h_;
    std::uint64_t s3 = h_ * l_;
    return 2 * (s1 + s2 + s3) + std::min({s1, s2, s3});
  }

  std::uint64_t l_;
  std::uint64_t w_;
  std::uint64_t h_;
};

auto main() -> int
{
  std::uint64_t total = 0;
  for (std::string line; std::getline(std::cin, line);) {
    Box b(line);
    auto amount = b.paper_needed();
    std::cout << "Box " << line << " needs " << amount << " sq.feet\n";
    total += amount;
  }
  std::cout << "Total = " << total << " square feet\n";

  return 0;
}