#include <array>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

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

std::multimap<Position, std::pair<Position, UInt>> costs{
  {'a', {'b', 1}}, {'b', {'a', 1}}, {'b', {'c', 2}}, {'b', {'h', 2}}, {'c', {'b', 2}},
  {'c', {'d', 2}}, {'c', {'h', 2}}, {'c', {'i', 2}}, {'d', {'c', 2}}, {'d', {'e', 2}},
  {'d', {'i', 2}}, {'d', {'j', 2}}, {'e', {'d', 2}}, {'e', {'f', 2}}, {'e', {'j', 2}},
  {'e', {'k', 2}}, {'f', {'e', 2}}, {'f', {'g', 1}}, {'f', {'k', 2}}, {'g', {'f', 1}},
  {'h', {'b', 2}}, {'h', {'c', 2}}, {'h', {'l', 1}}, {'i', {'c', 2}}, {'i', {'d', 2}},
  {'i', {'m', 1}}, {'j', {'d', 2}}, {'j', {'e', 2}}, {'j', {'n', 1}}, {'k', {'e', 2}},
  {'k', {'f', 2}}, {'k', {'o', 1}}, {'l', {'h', 1}}, {'m', {'i', 1}}, {'n', {'j', 1}},
  {'o', {'k', 1}}};

std::map<Type, UInt> multipliers{{'A', 1}, {'B', 10}, {'C', 100}, {'D', 1000}};

struct State
{
  auto finished() const noexcept -> bool { return nodes_ == finished_; }
  auto size() const noexcept -> UInt { return 15; }
  auto node(unsigned idx) noexcept -> Type& { return nodes_[idx]; }
  auto node(unsigned idx) const noexcept -> Type const& { return nodes_[idx]; }

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

private:
  std::array<Type, 15> nodes_{'.'};
  static std::array<Type, 15> finished_;
};

template<>
struct std::hash<State>
{
  auto operator()(State const& key) const -> std::size_t
  {
    std::size_t result{0};
    for (auto i{0}; i < 15; ++i) {
      result <<= 3;
      if (key.node(i) != '.') {
        result |= key.node(i) - 'A' + 1;
      }
    }
    return result;
  }
};

std::array<Type, 15> State::finished_ = {'.', '.', '.', '.', '.', '.', '.', 'A',
                                         'B', 'C', 'D', 'A', 'B', 'C', 'D'};

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
  State initial_state;

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line2_re)) {
    std::cerr << "Unable to match line 2 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 7; ++i) {
    initial_state.node(i) = m.str(i + 1)[0];
  }

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line3_re)) {
    std::cerr << "Unable to match line 3 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state.node(7 + i) = m.str(i + 1)[0];
  }

  std::getline(std::cin, line);
  if (!std::regex_search(line, m, line4_re)) {
    std::cerr << "Unable to match line 4 " << line << '\n';
    return 1;
  }
  for (unsigned i = 0; i < 4; ++i) {
    initial_state.node(11 + i) = m.str(i + 1)[0];
  }

  std::map<State, UInt> states;
  states.insert({initial_state, 0});
  std::set<State> visited;

  while (!states.empty()) {
    auto it{std::min_element(states.begin(), states.end(), [](auto const& lhs, auto const& rhs) {
      return lhs.second < rhs.second;
    })};
    if (visited.size() % 10'000 == 0) {
      std::cout << "Visited: " << visited.size() << " number of states: " << states.size()
                << " Min energy: " << it->second << '\n';
    }

    State state{it->first};
    UInt cost{it->second};
    visited.insert(state);
    states.erase(state);

    if (state.finished()) {
      std::cout << "Done with cost " << cost << '\n';
      return 0;
    }

    for (unsigned i = 0; i < state.size(); ++i) {
      if (state.node(i) == '.') {
        continue;
      }
      auto [it_begin, it_end] = costs.equal_range(i + 'a');
      for (auto cost_it{it_begin}; cost_it != it_end; ++cost_it) {
        State next_state{state};
        if (next_state.move(i, cost_it->second.first - 'a') && !visited.contains(next_state)) {
          UInt next_cost = cost + cost_it->second.second * multipliers[state.node(i)];
          auto [insert_it, success] = states.insert({next_state, next_cost});
          if (!success) {
            insert_it->second = std::min(insert_it->second, next_cost);
          }
        }
      }
    }
  }

  std::cerr << "FAILED\n";
  return 1;
}
