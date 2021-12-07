//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <array>
#include <iostream>
#include <set>
#include <string>
#include <utility>

#include <openssl/evp.h>

using MD5Digest = std::array<unsigned char, EVP_MAX_MD_SIZE>;

enum Directions { none = 0x0, up = 0x1, down = 0x2, left = 0x4, right = 0x8 };
auto md5_directions(std::string const& s) -> Directions
{
  MD5Digest digest;
  EVP_MD const* md{EVP_md5()};
  unsigned int md_len{0};

  EVP_MD_CTX* md_context{EVP_MD_CTX_new()};
  assert(md_context != nullptr);
  EVP_DigestInit_ex2(md_context, md, nullptr);
  EVP_DigestUpdate(md_context, s.data(), s.length());
  EVP_DigestFinal_ex(md_context, digest.data(), &md_len);
  Directions d{Directions::none};
  if (((digest[0] & 0xf0) >> 4) > 0xa) {
    d = static_cast<Directions>(d | Directions::up);
  }
  if ((digest[0] & 0xf) > 0xa) {
    d = static_cast<Directions>(d | Directions::down);
  }
  if (((digest[1] & 0xf0) >> 4) > 0xa) {
    d = static_cast<Directions>(d | Directions::left);
  }
  if ((digest[1] & 0xf) > 0xa) {
    d = static_cast<Directions>(d | Directions::right);
  }
  return d;
}

struct Node
{
  explicit Node(std::string s) : str_(std::move(s)) {}

  auto operator==(Node const& rhs) const noexcept -> bool { return str_ == rhs.str_; }

  auto operator<(Node const& rhs) const noexcept -> bool
  {
    return str_.size() < rhs.str_.size() || (str_.size() == rhs.str_.size() && str_ < rhs.str_);
  }

  [[nodiscard]] auto str() const -> std::string const& { return str_; }
  [[nodiscard]] auto x() const noexcept -> unsigned { return x_; }
  [[nodiscard]] auto y() const noexcept -> unsigned { return y_; }

  template<typename T>
  void insert_moves(T inserter) const
  {
    Directions d{md5_directions(str_)};
    if (x_ > 0 && (d & Directions::left) != Directions::none) {
      *inserter++ = Node{str_ + 'L', x_ - 1, y_};
    }
    if (y_ > 0 && (d & Directions::up) != Directions::none) {
      *inserter++ = Node{str_ + 'U', x_, y_ - 1};
    }
    if (x_ < 3 && (d & Directions::right) != Directions::none) {
      *inserter++ = Node{str_ + 'R', x_ + 1, y_};
    }
    if (y_ < 3 && (d & Directions::down) != Directions::none) {
      *inserter++ = Node{str_ + 'D', x_, y_ + 1};
    }
  }

private:
  Node(std::string s, unsigned x, unsigned y) : str_(std::move(s)), x_(x), y_(y) {}

  std::string str_;
  unsigned x_{0};
  unsigned y_{0};
};

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Failed to read line\n";
    return 1;
  }

  Node initial_state{line};

  std::set<Node> visited;
  std::set<Node> to_visit;
  to_visit.insert(initial_state);

  while (!to_visit.empty()) {
    auto it{to_visit.begin()};
    Node n{*it};
    to_visit.erase(it);
    visited.insert(n);
    if (n.x() == 3 && n.y() == 3) {
      std::cout << "Movements " << n.str().substr(line.size()) << '\n';
      return 0;
    }
    else {
      n.insert_moves(std::inserter(to_visit, to_visit.end()));
    }
  }

  std::cerr << "No solution.\n";
  return 1;
}
