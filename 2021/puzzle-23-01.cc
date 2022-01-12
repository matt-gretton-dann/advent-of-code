#include <array>
#include <cassert>
#include <functional>
#include <iostream>
#include <limits>
#include <map>
#include <regex>
#include <set>
#include <string>

#include "graph-utils.h"

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
// Actually : {0-6} can go to any of {7-10} at varying cost but there are move restritions.

using Position = char;
using UInt = int;
using Type = char;

std::multimap<Position, std::pair<std::vector<Position>, UInt>> valid_moves{
  {0, {{1, 7}, 3}},
  {0, {{1, 2, 8}, 5}},
  {0, {{1, 2, 3, 9}, 7}},
  {0, {{1, 2, 3, 4, 10}, 9}},
  {0, {{1, 7, 11}, 4}},
  {0, {{1, 2, 8, 12}, 6}},
  {0, {{1, 2, 3, 9, 13}, 8}},
  {0, {{1, 2, 3, 4, 10, 14}, 10}},
  {1, {{7}, 2}},
  {1, {{2, 8}, 4}},
  {1, {{2, 3, 9}, 6}},
  {1, {{2, 3, 4, 10}, 8}},
  {1, {{7, 11}, 3}},
  {1, {{2, 8, 12}, 5}},
  {1, {{2, 3, 9, 13}, 7}},
  {1, {{2, 3, 4, 10, 14}, 9}},
  {2, {{7}, 2}},
  {2, {{8}, 2}},
  {2, {{3, 9}, 4}},
  {2, {{3, 4, 10}, 6}},
  {2, {{7, 11}, 3}},
  {2, {{8, 12}, 3}},
  {2, {{3, 9, 13}, 5}},
  {2, {{3, 4, 10, 14}, 7}},
  {3, {{2, 7}, 4}},
  {3, {{8}, 2}},
  {3, {{9}, 2}},
  {3, {{4, 10}, 4}},
  {3, {{2, 7, 11}, 5}},
  {3, {{8, 12}, 3}},
  {3, {{9, 13}, 3}},
  {3, {{4, 10, 14}, 5}},
  {4, {{3, 2, 7}, 6}},
  {4, {{3, 8}, 4}},
  {4, {{9}, 2}},
  {4, {{10}, 2}},
  {4, {{3, 2, 7, 11}, 7}},
  {4, {{3, 8, 12}, 5}},
  {4, {{9, 13}, 3}},
  {4, {{10, 14}, 3}},
  {5, {{4, 3, 2, 7}, 8}},
  {5, {{4, 3, 8}, 6}},
  {5, {{4, 9}, 4}},
  {5, {{10}, 2}},
  {5, {{4, 3, 2, 7, 11}, 9}},
  {5, {{4, 3, 8, 12}, 7}},
  {5, {{4, 9, 13}, 5}},
  {5, {{10, 14}, 3}},
  {6, {{5, 4, 3, 2, 7}, 9}},
  {6, {{5, 4, 3, 8}, 7}},
  {6, {{5, 4, 9}, 5}},
  {6, {{5, 10}, 3}},
  {6, {{5, 4, 3, 2, 7, 11}, 10}},
  {6, {{5, 4, 3, 8, 12}, 8}},
  {6, {{5, 4, 9, 13}, 6}},
  {6, {{5, 10, 14}, 4}},

  {7, {{1, 0}, 3}},
  {7, {{1}, 2}},
  {7, {{2}, 2}},
  {7, {{2, 3}, 4}},
  {7, {{2, 3, 4}, 6}},
  {7, {{2, 3, 4, 5}, 8}},
  {7, {{2, 3, 4, 5, 6}, 9}},
  {8, {{2, 1, 0}, 5}},
  {8, {{2, 1}, 4}},
  {8, {{2}, 2}},
  {8, {{3}, 2}},
  {8, {{3, 4}, 4}},
  {8, {{3, 4, 5}, 6}},
  {8, {{3, 4, 5, 6}, 7}},
  {9, {{3, 2, 1, 0}, 7}},
  {9, {{3, 2, 1}, 6}},
  {9, {{3, 2}, 4}},
  {9, {{3}, 2}},
  {9, {{4}, 2}},
  {9, {{4, 5}, 4}},
  {9, {{4, 5, 6}, 5}},
  {10, {{4, 3, 2, 1, 0}, 9}},
  {10, {{4, 3, 2, 1}, 8}},
  {10, {{4, 3, 2}, 6}},
  {10, {{4, 3}, 4}},
  {10, {{4}, 2}},
  {10, {{5}, 2}},
  {10, {{5, 6}, 3}},
  {11, {{7, 1, 0}, 4}},
  {11, {{7, 1}, 3}},
  {11, {{7, 2}, 3}},
  {11, {{7, 2, 3}, 5}},
  {11, {{7, 2, 3, 4}, 7}},
  {11, {{7, 2, 3, 4, 5}, 9}},
  {11, {{7, 2, 3, 4, 5, 6}, 10}},
  {12, {{8, 2, 1, 0}, 6}},
  {12, {{8, 2, 1}, 5}},
  {12, {{8, 2}, 3}},
  {12, {{8, 3}, 3}},
  {12, {{8, 3, 4}, 5}},
  {12, {{8, 3, 4, 5}, 7}},
  {12, {{8, 3, 4, 5, 6}, 8}},
  {13, {{9, 3, 2, 1, 0}, 8}},
  {13, {{9, 3, 2, 1}, 7}},
  {13, {{9, 3, 2}, 5}},
  {13, {{9, 3}, 3}},
  {13, {{9, 4}, 3}},
  {13, {{9, 4, 5}, 5}},
  {13, {{9, 4, 5, 6}, 7}},
  {14, {{10, 4, 3, 2, 1, 0}, 10}},
  {14, {{10, 4, 3, 2, 1}, 9}},
  {14, {{10, 4, 3, 2}, 7}},
  {14, {{10, 4, 3}, 5}},
  {14, {{10, 4}, 3}},
  {14, {{10, 5}, 3}},
  {14, {{10, 5, 6}, 4}},
};

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
      // Only move out of the bottom row if we or the node below us is not meant to be there.
      return nodes_[from] != finished_[from] || nodes_[from + 4] != finished_[from];
    }
    if (from < 7) {
      if (nodes_[from] != finished_[to]) {
        return false;
      }
      while (to > 10) {
        to -= 4;
      }
      while (to < 15) {
        if (nodes_[to] != '.' && nodes_[to] != finished_[to]) {
          return false;
        }
        to += 4;
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
  std::array<Type, 15> nodes_{'.'};
  static std::array<Type, 15> finished_;
  UInt cost_{0};
};

std::array<Type, 15> State::finished_ = {'.', '.', '.', '.', '.', '.', '.', 'A',
                                         'B', 'C', 'D', 'A', 'B', 'C', 'D'};

std::ostream& operator<<(std::ostream& os, State const& s)
{
  os << "#############\n"
     << '#' << s.node(0) << s.node(1) << '.' << s.node(2) << '.' << s.node(3) << '.' << s.node(4)
     << '.' << s.node(5) << s.node(6) << "#\n"
     << "###" << s.node(7) << '#' << s.node(8) << '#' << s.node(9) << '#' << s.node(10) << "###\n"
     << "  #" << s.node(11) << '#' << s.node(12) << '#' << s.node(13) << '#' << s.node(14) << "#\n"
     << "  #########\n";

  return os;
}

struct StateTranstitionManager
{
  bool is_finished(State const& state) { return state.finished(); }

  template<typename Inserter>
  void generate_children(State const& state, Inserter inserter)
  {
    for (unsigned i = 0; i < state.size(); ++i) {
      if (state.node(i) == '.') {
        continue;
      }

      auto [it_begin, it_end] = valid_moves.equal_range(i);
      for (auto move_it{it_begin}; move_it != it_end; ++move_it) {
        State next_state(state);
        bool cont = true;
        for (auto pos_it : move_it->second.first) {
          if (state.node(pos_it) != '.') {
            cont = false;
            break;
          }
        }
        if (cont) {
          UInt cost_delta = move_it->second.second * multipliers[state.node(i)];
          if (next_state.move(i, move_it->second.first.back(), cost_delta)) {
            inserter(next_state, cost_delta);
          }
        }
      }
    }
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
  State initial_state{};

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

  auto [finished_state, finished_cost] =
    dijkstra(initial_state, UInt{0}, StateTranstitionManager{});
  std::cout << "Done with cost " << finished_cost << '\n';
  return 0;
}
