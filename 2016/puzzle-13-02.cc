//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <iostream>
#include <set>
#include <string>

using Num = unsigned long;

struct Node
{
  Node(Num x, Num y, Num cost) : x_(x), y_(y), cost_(cost) {}

  auto operator==(Node const& rhs) const noexcept -> bool { return x_ == rhs.x_ && y_ == rhs.y_; }

  auto operator<(Node const& rhs) const noexcept -> bool
  {
    return x_ < rhs.x_ || (x_ == rhs.x_ && y_ < rhs.y_);
  }

  Num x() const noexcept { return x_; }
  Num y() const noexcept { return y_; }
  Num cost() const noexcept { return cost_; }

  Node move_left() { return {x_ - 1, y_, cost_ + 1}; }
  Node move_up() { return {x_, y_ + 1, cost_ + 1}; }
  Node move_right() { return {x_ + 1, y_, cost_ + 1}; }
  Node move_down() { return {x_, y_ - 1, cost_ + 1}; }

private:
  Num x_;
  Num y_;
  Num cost_;
};

auto is_wall(Num num, Node const& node)
{
  Num value{num + node.x() * node.x() + 3 * node.x() + 2 * node.x() * node.y() + node.y() +
            node.y() * node.y()};
  Num bit_count{0};
  while (value != 0) {
    bit_count += value & 1;
    value >>= 1;
  }
  return ((bit_count & 1) == 1) ? true : false;
}

struct CostCompare
{
  constexpr auto operator()(Node const& lhs, Node const& rhs) const noexcept -> bool
  {
    return lhs.cost() < rhs.cost();
  }
};

using ToVisitSet = std::multiset<Node, CostCompare>;
using VisitedSet = std::set<Node>;

void update_to_visit(ToVisitSet& to_visit, VisitedSet const& visited, Num key, Node const& n)
{
  if (is_wall(key, n)) {
    return;
  }

  if (visited.contains(n)) {
    return;
  }

  if (n.cost() > 50) {
    return;
  }

  auto it{std::find(to_visit.begin(), to_visit.end(), n)};
  if (it == to_visit.end()) {
    to_visit.insert(n);
  }
  else if (n.cost() < it->cost()) {
    to_visit.erase(it);
    to_visit.insert(n);
  }
}

auto main() -> int
{
  VisitedSet visited;
  ToVisitSet to_visit;

  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read line.\n";
    return 1;
  }
  Num key{std::stoul(line)};

  to_visit.insert(Node{1, 1, 0});
  while (!to_visit.empty()) {
    auto it{to_visit.begin()};
    Node n(*it);

    to_visit.erase(it);
    visited.insert(n);
    if (n.x() > 0) {
      update_to_visit(to_visit, visited, key, n.move_left());
    }
    if (n.y() > 0) {
      update_to_visit(to_visit, visited, key, n.move_down());
    }
    update_to_visit(to_visit, visited, key, n.move_right());
    update_to_visit(to_visit, visited, key, n.move_up());
  }

  std::cout << "Number of visited locations: " << visited.size() << '\n';
  return 0;
}