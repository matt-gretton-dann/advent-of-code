#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>

// Map:
// 0 1    2    3   4    5 6
//      7    8    9  10
//     11   12   13  14
//     15   16   17  18
//     19   20   21  22

using Position = unsigned;
using UInt = unsigned;
using Type = char;

std::multimap<Position, std::pair<Position, UInt>> legal_moves{
  {0, {1, 1}},   {1, {0, 1}},   {1, {2, 2}},   {1, {7, 2}},   {2, {1, 2}},   {2, {3, 2}},
  {2, {7, 2}},   {2, {8, 2}},   {3, {2, 2}},   {3, {4, 2}},   {3, {8, 2}},   {3, {9, 2}},
  {4, {3, 2}},   {4, {5, 2}},   {4, {9, 2}},   {4, {10, 2}},  {5, {4, 2}},   {5, {6, 1}},
  {5, {10, 2}},  {6, {5, 1}},   {7, {1, 2}},   {7, {2, 2}},   {7, {11, 1}},  {8, {2, 2}},
  {8, {3, 2}},   {8, {12, 1}},  {9, {3, 2}},   {9, {4, 2}},   {9, {13, 1}},  {10, {4, 2}},
  {10, {5, 2}},  {10, {14, 1}}, {11, {7, 1}},  {12, {8, 1}},  {13, {9, 1}},  {14, {10, 1}},
  {11, {15, 1}}, {12, {16, 1}}, {13, {17, 1}}, {14, {18, 1}}, {15, {11, 1}}, {16, {12, 1}},
  {17, {13, 1}}, {18, {14, 1}}, {15, {19, 1}}, {16, {20, 1}}, {17, {21, 1}}, {18, {22, 1}},
  {19, {15, 1}}, {20, {16, 1}}, {21, {17, 1}}, {22, {18, 1}},
};

std::map<Type, UInt> multipliers{{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

struct State
{
  auto finished() const noexcept -> bool { return nodes_ == finished_; }
  auto size() const noexcept -> UInt { return node_size; }
  auto node(unsigned idx) noexcept -> Type& { return nodes_[idx]; }
  auto node(unsigned idx) const noexcept -> Type const& { return nodes_[idx]; }

  bool check_move(unsigned from, unsigned to)
  {
    if (nodes_[from] == '.' || nodes_[to] != '.') {
      return false;
    }
    if (from > 18) {
      // Bottom row
      // Only move out of the bottom row if we're not meant to be here.
      return nodes_[from] != finished_[from];
    }
    if (from > 14 && from < 19) {
      // Second bottom row
      if (to > 18) {
        // Only move down if we're the right node
        return nodes_[from] == finished_[to];
      }
      // Only move up if we or a node beneath us is incorrect.
      if (nodes_[from] != finished_[from]) {
        return true;
      }
      if (nodes_[from + 4] != '.' && nodes_[from + 4] != finished_[from]) {
        return true;
      }
      return false;
    }
    if (from > 10 && from < 15) {
      // Second top row
      if (to > 14) {
        // Only move down if we're the right node and the very bottom is empty or contains the
        // correct node.
        if (nodes_[from + 8] != '.' && nodes_[from + 8] != finished_[to]) {
          return false;
        }
        return nodes_[from] == finished_[to];
      }
      // Only move up if we or a node beneath us is incorrect.
      if (nodes_[from] != finished_[from]) {
        return true;
      }
      if (nodes_[from + 4] != '.' && nodes_[from + 4] != finished_[from]) {
        return true;
      }
      if (nodes_[from + 8] != '.' && nodes_[from + 8] != finished_[from]) {
        return true;
      }
      return false;
    }
    if (from > 6 && from < 11) {
      if (to > 10) {
        // Only move down if we're the right node and the bottom two rows are empty or contain the
        // correct node.
        if (nodes_[from + 8] != '.' && nodes_[from + 8] != finished_[to]) {
          return false;
        }
        if (nodes_[from + 12] != '.' && nodes_[from + 12] != finished_[to]) {
          return false;
        }
        return nodes_[from] == finished_[to];
      }
      if (nodes_[from] != finished_[from]) {
        return true;
      }
      if (nodes_[from + 4] != '.' && nodes_[from + 4] != finished_[from]) {
        return true;
      }
      if (nodes_[from + 8] != '.' && nodes_[from + 8] != finished_[from]) {
        return true;
      }
      if (nodes_[from + 12] != '.' && nodes_[from + 12] != finished_[from]) {
        return true;
      }
      return false;
    }
    if (from < 7) {
      if (to < 7) {
        // Can do any move along the row.
        return true;
      }
      // Can only move down if we're the right type.
      return nodes_[from] == finished_[to];
    }
    abort();
  }

  bool move(unsigned from, unsigned to)
  {
    if (!check_move(from, to)) {
      return false;
    }
    std::swap(nodes_[from], nodes_[to]);
    return true;
  }

  constexpr bool operator<(State const& rhs) const noexcept { return nodes_ < rhs.nodes_; }
  constexpr bool operator==(State const& rhs) const noexcept { return nodes_ < rhs.nodes_; }

  constexpr static unsigned node_size{23};

private:
  std::array<Type, node_size> nodes_{'.'};
  static std::array<Type, node_size> finished_;
};

std::array<Type, State::node_size> State::finished_ = {'.', '.', '.', '.', '.', '.', '.', 'A',
                                                       'B', 'C', 'D', 'A', 'B', 'C', 'D', 'A',
                                                       'B', 'C', 'D', 'A', 'B', 'C', 'D'};

struct StateCmp
{
  constexpr bool operator()(State const* l, State const* r) const noexcept
  {
    if (l == nullptr && r != nullptr) {
      return true;
    }
    if (r == nullptr) {
      return false;
    }
    return *l < *r;
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
  initial_state->node(11) = 'D';
  initial_state->node(12) = 'C';
  initial_state->node(13) = 'B';
  initial_state->node(14) = 'A';
  initial_state->node(15) = 'D';
  initial_state->node(16) = 'B';
  initial_state->node(17) = 'A';
  initial_state->node(18) = 'C';

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line4_re)) {
    std::cerr << "Unable to match line 4 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state->node(19 + i) = m.str(i + 1)[0];
  }

  std::map<State*, UInt, StateCmp> states;
  std::multimap<UInt, State*> costs;
  states.insert({initial_state, 0});
  costs.insert({0, initial_state});
  std::set<State*, StateCmp> visited;

  while (!states.empty()) {
    auto it{costs.begin()};
    if (visited.size() % 10'000 == 0) {
      std::cout << "Visited: " << visited.size() << " number of states: " << states.size()
                << " Min energy: " << it->first << '\n';
    }

    State* state{it->second};
    UInt cost{it->first};
    visited.insert(state);
    states.erase(state);
    costs.erase(it);

    if (state->finished()) {
      std::cout << "Done with cost " << cost << '\n';
      return 0;
    }

    for (unsigned i = 0; i < state->size(); ++i) {
      if (state->node(i) == '.') {
        continue;
      }
      auto [it_begin, it_end] = legal_moves.equal_range(i);
      for (auto move_it{it_begin}; move_it != it_end; ++move_it) {
        State* next_state = new State{*state};
        if (next_state->move(i, move_it->second.first) && !visited.contains(next_state)) {
          UInt next_cost = cost + move_it->second.second * multipliers[state->node(i)];
          auto [insert_it, success] = states.insert({next_state, next_cost});
          if (!success) {
            if (next_cost < insert_it->second) {
              insert_it->second = next_cost;
              auto [cost_begin, cost_end] = costs.equal_range(cost);
              while (cost_begin != cost_end) {
                if (*cost_begin->second == *next_state) {
                  delete next_state;
                  next_state = cost_begin->second;
                  costs.erase(cost_begin);
                  break;
                }
                ++cost_begin;
              }
              costs.insert({next_cost, next_state});
            }
            else {
              delete next_state;
            }
          }
          else {
            costs.insert({next_cost, next_state});
          }
        }
        else {
          delete next_state;
        }
      }
    }
  }

  std::cerr << "FAILED\n";
  return 1;
}
