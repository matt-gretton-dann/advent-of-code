#include <array>
#include <cassert>
#include <iostream>
#include <regex>
#include <string>

using Int = long;

struct Vector
{
  Vector(Int x, Int y, Int z)
    : x_(x), y_(y), z_(z)
  {
  }

  Int x() const noexcept { return x_; }
  Int y() const noexcept { return y_; }
  Int z() const noexcept { return z_; }

  auto operator+=(Vector const& rhs) noexcept
  {
    x_ += rhs.x();
    y_ += rhs.y();
    z_ += rhs.z();
    return *this;
  }

private:
  Int x_;
  Int y_;
  Int z_;
};

auto operator<(Vector const& lhs, Vector const& rhs) -> bool
{
  if (lhs.x() < rhs.x()) {
    return true;
  }
  if (lhs.x() == rhs.x() && lhs.y() < rhs.y()) {
    return true;
  }
  if (lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() < rhs.z()) {
    return true;
  }
  return false;
}

auto operator==(Vector const& lhs, Vector const& rhs) -> bool
{
  return lhs.x() == rhs.x() && lhs.y() == rhs.y() && lhs.z() == rhs.z();
}

struct NodeState
{
  NodeState(Vector const& pos, Vector const& velocity, Vector const& accel)
    : pos_(pos), velocity_(velocity), accel_(accel)
  {
  }

  void update_position()
  {
    velocity_ += accel_;
    pos_ += velocity_;
  }

  auto position() const noexcept -> Vector const& { return pos_; }

private:
  Vector pos_;
  Vector velocity_;
  Vector accel_;
};

auto operator==(NodeState const& lhs, NodeState const& rhs) -> bool
{
  return lhs.position() == rhs.position();
}

auto operator<(NodeState const& lhs, NodeState const& rhs) -> bool
{
  return lhs.position() < rhs.position();
}

auto main() -> int
{
  static std::regex re{
    R"(p=<(-?\d+),(-?\d+),(-?\d+)>, v=<(-?\d+),(-?\d+),(-?\d+)>, a=<(-?\d+),(-?\d+),(-?\d+)>)"};
  std::vector<NodeState> nodes;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << '\n';
      return 1;
    }
    Vector pos{std::stol(m.str(1)), std::stol(m.str(2)), std::stol(m.str(3))};
    Vector velocity{std::stol(m.str(4)), std::stol(m.str(5)), std::stol(m.str(6))};
    Vector accel{std::stol(m.str(7)), std::stol(m.str(8)), std::stol(m.str(9))};
    nodes.push_back(NodeState{pos, velocity, accel});
  }

  for (unsigned i{0}; i < 1000; ++i) {
    std::sort(nodes.begin(), nodes.end());
    for (auto it{nodes.begin()}; it != nodes.end();) {
      auto it2{it};
      while (it2 != nodes.end() && *it == *it2) {
        ++it2;
      }
      if (std::distance(it, it2) > 1) {
        it = nodes.erase(it, it2);
      }
      else {
        assert(std::distance(it, it2) == 1);
        it = it2;
      }
    }

    std::for_each(nodes.begin(), nodes.end(), [](auto& node) { node.update_position(); });
  }

  std::cout << "Number of nodes left: " << nodes.size() << '\n';
}
