#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>

struct Box
{
  /** Construct box.
   *  \param s String representation of dimensions `lxwxh`
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
    auto s1{l_ * w_};
    auto s2{w_ * h_};
    auto s3{h_ * l_};
    return 2 * (s1 + s2 + s3) + std::min({s1, s2, s3});
  }

  // How much ribbon do we need?
  [[nodiscard]] auto ribbon_needed() const -> std::uint64_t
  {
    // The various side perimeters - we want the min of these multiplied by
    // volume.
    auto p1{2 * (l_ + w_)};
    auto p2{2 * (w_ + h_)};
    auto p3{2 * (h_ + l_)};
    return l_ * w_ * h_ + std::min({p1, p2, p3});
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
    auto amount = b.ribbon_needed();
    std::cout << "Box " << line << " needs " << amount << " feet of ribbon\n";
    total += amount;
  }
  std::cout << "Total = " << total << " feet of ribbon\n";

  return 0;
}