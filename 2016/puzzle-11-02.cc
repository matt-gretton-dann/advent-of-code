//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <algorithm>
#include <array>
#include <iostream>
#include <ostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

/** \brief  Item types.  */
enum class ItemType { generator, chip };

/** \brief An Item - consists of a type and a name
 *
 * To improve performance we store all names in a lookup table and use IDs to map to strings.  This
 * makes comparing items a couple of integer comparisons and not a string compare.
 */
struct Item
{
  /**
   * \brief      Constructor
   * \param type Item type
   * \param name Item name
   */
  Item(ItemType type, std::string const& name) : type_(type)
  {
    auto it{std::find(names_.begin(), names_.end(), name)};
    if (it == names_.end()) {
      name_index_ = names_.size();
      names_.push_back(name);
    }
    else {
      name_index_ = it - names_.begin();
    }
  }

  /**
   * \brief      Less than comparator
   * \param  rhs Right-hand side
   * \return     \c true if \c *this < \a rhs.
   */
  [[nodiscard]] auto operator<(Item const& rhs) const noexcept -> bool
  {
    return (type_ < rhs.type_) || (type_ == rhs.type_ && name_index_ < rhs.name_index_);
  }

  /**
   * \brief      Equality comparator
   * \param  rhs Right hand side
   * \return     \c true iff \c *this == \a rhs
   */
  [[nodiscard]] auto operator==(Item const& rhs) const noexcept -> bool
  {
    return name_index_ == rhs.name_index_ && type_ == rhs.type_;
  }

  ItemType type_{ItemType::generator};
  size_t name_index_{0};
  static std::vector<std::string> names_;
};

std::vector<std::string> Item::names_;

/**
 * \brief A possible state to examine.
 *
 * Consists of the current floor of the elevator and floors containing items.
 */
struct State
{
  static constexpr unsigned floor_count{4};  ///< Number of floors

  /**
   * \brief      Less than comparator
   * \param  rhs Right-hand side
   * \return     \c true if \c *this < \a rhs.
   */
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

  /**
   * \brief      Equality comparator
   * \param  rhs Right hand side
   * \return     \c true iff \c *this == \a rhs
   */
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

  /**
   * \brief   Are all items at the top.
   * \return  \c true iff the elevator and all items are on the top floor.
   */
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

  /**
   * \brief       Move an item from the current floor
   * \param  dir  Direction to move item
   * \param  item Item to move
   * \return      New state with item and elevator moved.
   */
  [[nodiscard]] auto move(int dir, Item const& item) const -> State
  {
    State n{*this};
    n.floors_[n.elevator_floor_].erase(item);
    n.elevator_floor_ += dir;
    n.floors_[n.elevator_floor_].insert(item);
    return n;
  }

  /**
   * \brief       Move two items from the current floor
   * \param  dir  Direction to move item
   * \param  i1   Item to move
   * \param  i2   Item to move
   * \return      New state with items and elevator moved.
   */
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

  /**
   * \brief  Is this a valid state?
   * \return \c true iff this state is valid.
   */
  [[nodiscard]] auto valid() const noexcept -> bool
  {
    for (auto const& floor : floors_) {
      for (auto item : floor) {
        if (item.type_ != ItemType::chip) {
          continue;
        }
        auto name = item.name_index_;
        bool has_generator{false};
        bool has_other_generators{false};
        for (auto i2 : floor) {
          if (i2.type_ == ItemType::generator && i2.name_index_ == name) {
            has_generator = true;
          }
          if (i2.type_ == ItemType::generator && i2.name_index_ != name) {
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

  /** Insert an item onto a given floor. */
  void insert_item(unsigned floor, Item const& item) { floors_.at(floor).insert(item); }

  using FloorState = std::set<Item>;
  unsigned elevator_floor_{0};                  ///< Current floor
  std::array<FloorState, floor_count> floors_;  ///< Items on each floor.
};

/** Translate a floor name to a number.  Note we translate to 0-based from  1-based.  */
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

/**
 * \brief          Update \a to_visit states with \a s if it is valid and hasn't been seen before.
 * \param visited  Previously visited states
 * \param to_visit States to visit next iteration.
 * \param s        State to add
 */
void update_sets(std::set<State> const& visited, std::set<State>& to_visit, State const& s)
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

  /* Read standard input to get the initial state.  */
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
  initial_state.insert_item(0, {ItemType::generator, "elerium"});
  initial_state.insert_item(0, {ItemType::chip, "elerium"});
  initial_state.insert_item(0, {ItemType::generator, "dilithium"});
  initial_state.insert_item(0, {ItemType::chip, "dilithium"});

  /* This is basically Dijkstra's minimum distance graph algorithm.  However, there are some slight
   * optimisations to make this perform acceptably.
   *
   * Because the cost function is the number of moves of the elevator we can iterate through all
   * States with cost N, knowing that every new state from those will have cost N + 1.
   *
   * So we have the set \c to_visit which is the set of states that cost \c cost to reach.  We
   * iterate over every state in that producing the states with cost \c cost + 1.  These go into
   * the set \c next_to_visit.  We ensure there are no duplicates of previously visited states
   * in that set, and then repeat the process.
   */
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