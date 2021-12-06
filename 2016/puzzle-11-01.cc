//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <array>
#include <iostream>
#include <ostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

enum class ItemType { generator, chip };

struct Item
{
  auto operator<(Item const& rhs) const noexcept -> bool
  {
    if (name_ < rhs.name_) {
      return true;
    }
    if (name_ == rhs.name_ && type_ < rhs.type_) {
      return true;
    }
    return false;
  }

  auto operator==(Item const& rhs) const noexcept -> bool
  {
    return name_ == rhs.name_ && type_ == rhs.type_;
  }

  ItemType type_{ItemType::generator};
  std::string name_{};
};

auto operator<<(std::ostream& os, Item const& item) -> std::ostream&
{
  switch (item.type_) {
  case ItemType::generator:
    os << "G";
    break;
  case ItemType::chip:
    os << "C";
    break;
  default:
    break;
  }
  os << item.name_;
  return os;
}

struct State
{
  static constexpr unsigned floor_count{4};

  [[nodiscard]] auto operator==(State const& rhs) const noexcept -> bool
  {
    if (elevator_floor_ != rhs.elevator_floor_) {
      return false;
    }
    for (unsigned i{0}; i < floor_count; ++i) {
      if (floors_[i] != rhs.floors_[i]) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] auto operator<(State const& rhs) const noexcept -> bool
  {
    if (elevator_floor_ < rhs.elevator_floor_) {
      return true;
    }
    if (elevator_floor_ > rhs.elevator_floor_) {
      return false;
    }
    for (unsigned i{0}; i < floor_count; ++i) {
      if (floors_[i] < rhs.floors_[i]) {
        return true;
      }
      if (floors_[i] > rhs.floors_[i]) {
        return false;
      }
    }
    return false;
  }

  [[nodiscard]] auto all_at_top() const noexcept -> bool
  {
    if (elevator_floor_ != floor_count - 1) {
      return false;
    }
    for (unsigned i{0}; i < floor_count - 1; ++i) {
      if (!floors_[i].empty()) {
        return false;
      }
    }
    return true;
  }

  [[nodiscard]] auto move(int dir, Item const& item) const -> State
  {
    State n{*this};
    n.floors_[n.elevator_floor_].erase(item);
    n.elevator_floor_ += dir;
    n.floors_[n.elevator_floor_].insert(item);
    return n;
  }

  [[nodiscard]] auto move(int dir, Item const& i1, Item const& i2) const -> State
  {
    State n{*this};
    n.floors_[n.elevator_floor_].erase(i1);
    n.floors_[n.elevator_floor_].erase(i2);
    n.elevator_floor_ += dir;
    n.floors_[n.elevator_floor_].insert(i1);
    n.floors_[n.elevator_floor_].insert(i2);
    return n;
  }

  [[nodiscard]] auto valid() const noexcept -> bool
  {
    for (auto floor : floors_) {
      for (auto item : floor) {
        if (item.type_ != ItemType::chip) {
          continue;
        }
        auto name = item.name_;
        bool has_generator{false};
        bool has_other_generators{false};
        for (auto i2 : floor) {
          if (i2.type_ == ItemType::generator && i2.name_ == name) {
            has_generator = true;
          }
          if (i2.type_ == ItemType::generator && i2.name_ != name) {
            has_other_generators = true;
          }
        }
        if (has_other_generators && !has_generator) {
          return false;
        }
      }
    }
    return true;
  }

  void insert_item(unsigned floor, Item const& item) { floors_.at(floor).insert(item); }

  void print(std::ostream& os) const
  {
    unsigned floor{floor_count};
    while (floor-- != 0) {
      std::cout << ((elevator_floor_ == floor) ? 'E' : '-');
      for (auto const& it : floors_.at(floor)) {
        os << ' ' << it;
      }
      os << '\n';
    }
  }

  using FloorState = std::set<Item>;
  unsigned elevator_floor_{0};
  std::array<FloorState, floor_count> floors_;
};

auto to_floor(std::string const& s) -> unsigned
{
  if (s == "first") {
    return 0;
  }
  if (s == "second") {
    return 1;
  }
  if (s == "third") {
    return 2;
  }
  if (s == "fourth") {
    return 3;
  }
  abort();
}

auto update_sets(std::set<State> const& visited, std::set<State>& to_visit, State const& s)
{
  if (s.valid() && visited.find(s) == visited.end()) {
    to_visit.insert(s);
  }
}

auto main() -> int
{
  std::string line;
  static std::regex floor_re{"^The ([a-z]+) floor contains "};
  static std::regex generator_re{"([a-z]+) generator"};
  static std::regex microchip_re{"([a-z]+)-compatible microchip"};
  State initial_state;

  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_search(line, m, floor_re)) {
      std::cerr << "unable to interpret beginning";
      return 1;
    }
    unsigned floor{to_floor(m.str(1))};
    std::string generators{m.suffix()};
    std::string microchips{m.suffix()};
    while (!generators.empty()) {
      if (!std::regex_search(generators, m, generator_re)) {
        break;
      }
      initial_state.insert_item(floor, {ItemType::generator, m.str(1)});
      generators = m.suffix();
    }
    while (!microchips.empty()) {
      if (!std::regex_search(microchips, m, microchip_re)) {
        break;
      }
      initial_state.insert_item(floor, {ItemType::chip, m.str(1)});
      microchips = m.suffix();
    }
  }

  std::set<State> visited;
  std::set<State> to_visit;
  to_visit.insert(initial_state);
  unsigned cost{0};

  while (true) {
    std::cout << "Cost: " << cost << " have visited: " << visited.size()
              << " visiting this time: " << to_visit.size() << '\n';
    if (to_visit.empty()) {
      std::cout << "No solution found.\n";
      return 1;
    }
    std::set<State> next_to_visit;
    std::copy(to_visit.begin(), to_visit.end(), std::inserter(visited, visited.end()));
    for (auto const& state : to_visit) {
      if (state.all_at_top()) {
        std::cout << "Done in " << cost << " moves\n";
        return 0;
      }

      auto current_floor{state.elevator_floor_};
      for (auto i1{state.floors_[current_floor].begin()}; i1 != state.floors_[current_floor].end();
           ++i1) {
        if (current_floor < State::floor_count - 1) {
          update_sets(visited, next_to_visit, state.move(1, *i1));
          auto i2{i1};
          ++i2;
          for (; i2 != state.floors_[current_floor].end(); ++i2) {
            update_sets(visited, next_to_visit, state.move(1, *i1, *i2));
          }
        }
        if (current_floor > 0) {
          update_sets(visited, next_to_visit, state.move(-1, *i1));
          auto i2{i1};
          ++i2;
          for (; i2 != state.floors_[current_floor].end(); ++i2) {
            update_sets(visited, next_to_visit, state.move(-1, *i1, *i2));
          }
        }
      }
    }
    std::swap(to_visit, next_to_visit);
    ++cost;
  }
}