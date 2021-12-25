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
//  0 -1- 1 -2- 2 -2- 3 -2- 4 -2- 5 -1- 6
//         \   / \   / \   / \   /
//          2 2   2 2   2 2   2 2
//           +     +     +     +
//           7     8     9    10
//           |     |     |     |
//           1     1     1     1
//           |     |     |     |
//          11    12    13    14
//           |     |     |     |
//           1     1     1     1
//           |     |     |     |
//          15    16    17    18
//           |     |     |     |
//           1     1     1     1
//           |     |     |     |
//          19    20    21    22
// Actually : {0-6} can go to any of {7-10} at varying cost but there are move restritions.

using Position = char;
using UInt = int;
using Type = char;

std::multimap<Position, std::pair<Position, UInt>> valid_moves{
  {0, {7, 3}},   {0, {8, 5}},   {0, {9, 7}},   {0, {10, 9}},  {1, {7, 2}},   {1, {8, 4}},
  {1, {9, 6}},   {1, {10, 8}},  {2, {7, 2}},   {2, {8, 2}},   {2, {9, 4}},   {2, {10, 6}},
  {3, {7, 4}},   {3, {8, 2}},   {3, {9, 2}},   {3, {10, 4}},  {4, {7, 6}},   {4, {8, 4}},
  {4, {9, 2}},   {4, {10, 2}},  {5, {7, 8}},   {5, {8, 6}},   {5, {9, 4}},   {5, {10, 2}},
  {6, {7, 9}},   {6, {8, 7}},   {6, {9, 5}},   {6, {10, 3}},  {7, {0, 3}},   {7, {1, 2}},
  {7, {2, 2}},   {7, {3, 4}},   {7, {4, 6}},   {7, {5, 8}},   {7, {6, 9}},   {7, {11, 1}},
  {8, {0, 5}},   {8, {1, 4}},   {8, {2, 2}},   {8, {3, 2}},   {8, {4, 4}},   {8, {5, 6}},
  {8, {6, 7}},   {8, {12, 1}},  {9, {0, 7}},   {9, {1, 6}},   {9, {2, 4}},   {9, {3, 2}},
  {9, {4, 2}},   {9, {5, 4}},   {9, {6, 5}},   {9, {13, 1}},  {10, {0, 9}},  {10, {1, 8}},
  {10, {2, 6}},  {10, {3, 4}},  {10, {4, 2}},  {10, {5, 2}},  {10, {6, 3}},  {10, {14, 1}},
  {11, {7, 1}},  {12, {8, 1}},  {13, {9, 1}},  {14, {10, 1}}, {11, {15, 1}}, {12, {16, 1}},
  {13, {17, 1}}, {14, {18, 1}}, {15, {11, 1}}, {16, {12, 1}}, {17, {13, 1}}, {18, {14, 1}},
  {14, {10, 1}}, {15, {19, 1}}, {16, {20, 1}}, {17, {21, 1}}, {18, {22, 1}}, {19, {15, 1}},
  {20, {16, 1}}, {21, {17, 1}}, {22, {18, 1}}};

std::map<Type, UInt> multipliers{{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

struct State
{
  static constexpr UInt size_ = 23;

  auto finished() const noexcept -> bool { return nodes_ == finished_; }
  auto size() const noexcept -> UInt { return size_; }
  auto node(unsigned idx) noexcept -> Type& { return nodes_[idx]; }
  auto node(unsigned idx) const noexcept -> Type const& { return nodes_[idx]; }
  auto cost() const noexcept -> UInt { return cost_; }

  bool check_move(unsigned from, unsigned to)
  {
    if (nodes_[from] == '.' || nodes_[to] != '.') {
      return false;
    }
    if (from > 18) {
      // Only move out of the bottom row if we're not meant to be here.
      return nodes_[from] != finished_[from];
    }
    if (from > 14 && from < 19) {
      if (to < 15) {
        // Second bottom row - only move up if we're not meant to be here or the one below isn't.
        return nodes_[from] != finished_[from] ||
               (nodes_[from + 4] != '.' && nodes_[from + 4] != finished_[from + 4]);
      }

      // Moving down - only do it if we're of the right type.
      return nodes_[from] == finished_[to];
    }
    if (from > 10 && from < 15) {
      // Second top row.
      if (to < 11) {
        return nodes_[from] != finished_[from] ||
               (nodes_[from + 4] != '.' && nodes_[from + 4] != finished_[from + 4]) ||
               (nodes_[from + 8] != '.' && nodes_[from + 8] != finished_[from + 8]);
      }

      // Moving down - only do it if we're of the right type.
      return nodes_[from] == finished_[to];
    }
    if (from > 6 && from < 11) {
      if (to > 10) {
        // Only move into bottom position if we're moving the correct piece.
        return nodes_[from] == finished_[to];
      }
      // Moving to top row
      if (to == 0 && nodes_[1] != '.') {
        return false;
      }
      if (to < 2 && from > 7 && nodes_[2] != '.') {
        return false;
      }
      if (to < 3 && from > 8 && nodes_[3] != '.') {
        return false;
      }
      if (to < 4 && from > 9 && nodes_[4] != '.') {
        return false;
      }

      if (to == 6 && nodes_[5] != '.') {
        return false;
      }
      if (to > 4 && from < 10 && nodes_[4] != '.') {
        return false;
      }
      if (to > 3 && from < 9 && nodes_[3] != '.') {
        return false;
      }
      if (to > 2 && from < 8 && nodes_[2] != '.') {
        return false;
      }

      return true;
    }
    if (from < 7) {
      // Can only move down if we're the right type.
      if (nodes_[from] != finished_[to]) {
        return false;
      }

      // Now encode the rules about moving along the top.
      if (from == 0 && nodes_[1] != '.') {
        return false;
      }
      if (from < 2 && to > 7 && nodes_[2] != '.') {
        return false;
      }
      if (from < 3 && to > 8 && nodes_[3] != '.') {
        return false;
      }
      if (from < 4 && to > 9 && nodes_[4] != '.') {
        return false;
      }

      if (from == 6 && nodes_[5] != '.') {
        return false;
      }
      if (from > 4 && to < 10 && nodes_[4] != '.') {
        return false;
      }
      if (from > 3 && to < 9 && nodes_[3] != '.') {
        return false;
      }
      if (from > 2 && to < 8 && nodes_[2] != '.') {
        return false;
      }

      return true;
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
  std::array<Type, size_> nodes_{'.'};
  static std::array<Type, size_> finished_;
  UInt cost_{0};
};

std::array<Type, State::size_> State::finished_ = {'.', '.', '.', '.', '.', '.', '.', 'A',
                                                   'B', 'C', 'D', 'A', 'B', 'C', 'D', 'A',
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

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line4_re)) {
    std::cerr << "Unable to match line 5 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state->node(15 + i) = m.str(i + 1)[0];
  }

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line4_re)) {
    std::cerr << "Unable to match line 6 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state->node(19 + i) = m.str(i + 1)[0];
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
