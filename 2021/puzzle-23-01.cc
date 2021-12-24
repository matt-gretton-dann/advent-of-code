#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>

// Map:
// #############
// #ab.c.d.e.fg#
// ###h#i#j#k###
//   #l#m#n#o#
//   #########
//
//  a -1- b -2- c -2- d -2- e -2- f -1- g
//         \   / \   / \   / \   /
//          2 2   2 2   2 2   2 2
//           +     +     +     +
//           h     i     j     k
//           |     |     |     |
//           1     1     1     1
//           |     |     |     |
//           l     m     n     o

using Position = char;
using UInt = int;
using Type = char;

std::multimap<Position, std::pair<Position, UInt>> valid_moves{
  {0, {1, 1}},  {1, {0, 1}},   {1, {2, 2}},  {1, {7, 2}},  {2, {1, 2}},  {2, {3, 2}},
  {2, {7, 2}},  {2, {8, 2}},   {3, {2, 2}},  {3, {4, 2}},  {3, {8, 2}},  {3, {9, 2}},
  {4, {3, 2}},  {4, {5, 2}},   {4, {9, 2}},  {4, {10, 2}}, {5, {4, 2}},  {5, {6, 1}},
  {5, {10, 2}}, {6, {5, 1}},   {7, {1, 2}},  {7, {2, 2}},  {7, {11, 1}}, {8, {2, 2}},
  {8, {3, 2}},  {8, {12, 1}},  {9, {3, 2}},  {9, {4, 2}},  {9, {13, 1}}, {10, {4, 2}},
  {10, {5, 2}}, {10, {14, 1}}, {11, {7, 1}}, {12, {8, 1}}, {13, {9, 1}}, {14, {10, 1}}};

std::map<Type, UInt> multipliers{{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

struct State
{
  auto finished() const noexcept -> bool { return nodes_ == finished_; }
  auto size() const noexcept -> UInt { return 15; }
  auto node(unsigned idx) noexcept -> Type& { return nodes_[idx]; }
  auto node(unsigned idx) const noexcept -> Type const& { return nodes_[idx]; }
  auto cost() const noexcept -> UInt { return cost_; }

  bool check_move(unsigned from, unsigned to)
  {
    if (nodes_[from] == '.' || nodes_[to] != '.') {
      return false;
    }
    if (from > 10) {
      // Only move out of the bottom row if we're not meant to be here.
      return nodes_[from] != finished_[from];
    }
    if (from > 6 && from < 11) {
      if (to > 10) {
        // Only move into bottom position if we're moving the correct piece.
        return nodes_[from] == finished_[to];
      }
      // Moving to top row - anything allowed.
      return true;
    }
    if (from < 7) {
      if (to < 7) {
        return true;
      }
      // Can only move down if we're the right type.
      return nodes_[from] == finished_[to];
    }
    abort();
  }

  bool move(unsigned from, unsigned to, UInt cost)
  {
    if (!check_move(from, to)) {
      return false;
    }
    std::swap(nodes_[from], nodes_[to]);
    cost_ += cost;
    return true;
  }

  bool operator<(State const& rhs) const noexcept { return nodes_ < rhs.nodes_; }
  bool operator==(State const& rhs) const noexcept { return nodes_ < rhs.nodes_; }

private:
  std::array<Type, 15> nodes_{'.'};
  static std::array<Type, 15> finished_;
  UInt cost_{0};
};

std::array<Type, 15> State::finished_ = {'.', '.', '.', '.', '.', '.', '.', 'A',
                                         'B', 'C', 'D', 'A', 'B', 'C', 'D'};

struct StateCmp
{
  bool operator()(State const* lhs, State const* rhs) const noexcept
  {
    if (lhs == nullptr && rhs != nullptr) {
      return true;
    }
    if (rhs == nullptr) {
      return true;
    }
    return *lhs < *rhs;
  }
};

struct CostStateCmp
{
  bool operator()(State const* lhs, State const* rhs) const noexcept
  {
    if (lhs == nullptr && rhs != nullptr) {
      return true;
    }
    if (rhs == nullptr) {
      return true;
    }
    return lhs->cost() < rhs->cost() || (lhs->cost() == rhs->cost() && *lhs < *rhs);
  }
};

auto main() -> int
{
  std::regex line2_re{R"(#(.)(.)\.(.)\.(.)\.(.)\.(.)(.)#)"};
  std::regex line3_re{"###(.)#(.)#(.)#(.)###"};
  std::regex line4_re{"#(.)#(.)#(.)#(.)#"};

  std::string line;
  std::smatch m;
  std::getline(std::cin, line);
  if (line != "#############") {
    std::cerr << "Incorrect first line.\n";
    return 1;
  }
  State* initial_state = new State;

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line2_re)) {
    std::cerr << "Unable to match line 2 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 7; ++i) {
    initial_state->node(i) = m.str(i + 1)[0];
  }

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line3_re)) {
    std::cerr << "Unable to match line 3 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state->node(7 + i) = m.str(i + 1)[0];
  }

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line4_re)) {
    std::cerr << "Unable to match line 4 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state->node(11 + i) = m.str(i + 1)[0];
  }

  std::set<State*, StateCmp> states;
  std::set<State*, CostStateCmp> costs;
  std::set<State*, StateCmp> visited;
  states.insert(initial_state);
  costs.insert(initial_state);

  while (!costs.empty()) {
    assert(costs.size() == states.size());
    auto it{costs.begin()};

    State* state{*it};
    visited.insert(state);
    states.erase(state);
    costs.erase(it);

    if (visited.size() % 10'000 == 0) {
      std::cout << "Visited: " << visited.size() << " number of states: " << states.size()
                << " Min energy: " << state->cost() << '\n';
    }

    if (state->finished()) {
      std::cout << "Done with cost " << state->cost() << '\n';
      return 0;
    }

    for (unsigned i = 0; i < state->size(); ++i) {
      if (state->node(i) == '.') {
        continue;
      }
      auto [it_begin, it_end] = valid_moves.equal_range(i);
      for (auto move_it{it_begin}; move_it != it_end; ++move_it) {
        State* next_state = new State{*state};
        UInt cost_delta = move_it->second.second * multipliers[state->node(i)];
        bool keep{false};
        if (next_state->move(i, move_it->second.first, cost_delta) &&
            !visited.contains(next_state)) {
          auto [insert_it, success] = states.insert(next_state);
          if (!success) {
            auto old_state{*insert_it};
            if (next_state->cost() < old_state->cost()) {
              keep = true;
              costs.erase(old_state);
              states.erase(old_state);
              states.insert(next_state);
              costs.insert(next_state);
            }
          }
          else {
            keep = true;
            costs.insert(next_state);
          }
        }
        if (!keep) {
          delete next_state;
        }
      }
    }
  }

  std::cerr << "FAILED\n";
  return 1;
}
