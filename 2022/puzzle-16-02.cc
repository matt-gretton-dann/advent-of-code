//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <regex>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

using namespace std::string_literals;

struct ValveID
{
  explicit constexpr ValveID(std::string const& id) noexcept : id_((id[0] - 'A') * 26 + id[1] - 'A')
  {
  }
  explicit constexpr ValveID(UInt i) noexcept : id_(i) {}
  constexpr ValveID() noexcept = default;
  constexpr ValveID(ValveID const&) noexcept = default;
  constexpr auto operator=(ValveID const&) noexcept -> ValveID& = default;
  constexpr ValveID(ValveID&&) noexcept = default;
  constexpr auto operator=(ValveID&&) noexcept -> ValveID& = default;
  constexpr ~ValveID() noexcept = default;
  constexpr operator std::size_t() const noexcept { return id_; }

  constexpr auto operator<=>(ValveID const& r) const noexcept -> std::strong_ordering
  {
    return id_ <=> r.id_;
  }

  static constexpr auto max() -> ValveID
  {
    ValveID id;
    id.id_ = 26 * 26;
    return id;
  }

private:
  friend auto operator<<(std::ostream& os, ValveID id) -> std::ostream&;
  std::uint32_t id_{std::numeric_limits<std::uint32_t>::max()};
};

auto operator<<(std::ostream& os, ValveID id) -> std::ostream&
{
  return os << static_cast<char>((id.id_ / 26) + 'A') << static_cast<char>((id.id_ % 26) + 'A');
}

struct State
{
  ValveID id_[2]{ValveID("AA"s), ValveID("AA"s)};
  UInt next_time_[2]{26, 26};
  UInt total_rate_{0};
  std::vector<char> open_{std::vector<char>(ValveID::max(), false)};

  auto operator<=>(State const& rhs) const noexcept -> std::strong_ordering
  {
    for (unsigned i = 0; i < 2; ++i) {
      if (id_[i] != rhs.id_[i]) {
        return id_[i] <=> rhs.id_[i];
      }
      if (next_time_[i] != rhs.next_time_[i]) {
        return next_time_[i] <=> rhs.next_time_[i];
      }
    }
    for (UInt i{0}; i < open_.size(); ++i) {
      if (open_[i] != rhs.open_[i]) {
        return open_[i] <=> rhs.open_[i];
      }
    }
    return std::strong_ordering::equal;
  }
};

auto main() -> int
{
  std::string line;
  std::regex const re{
    "Valve ([A-Z][A-Z]) has flow rate=(\\d+); tunnels? leads? to valves? ([A-Z, ]+)"};
  std::vector<std::vector<UInt>> edges(ValveID::max(), std::vector<UInt>(ValveID::max(), 0));
  std::vector<UInt> rates(ValveID::max(), 0);
  UInt size{0};

  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Cannot interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    ValveID const from{m.str(1)};
    rates.at(from) = std::stoull(m.str(2));
    std::string edge{};
    for (auto c : m.str(3)) {
      if (c == ',' || c == ' ') {
        continue;
      }
      edge += c;
      if (edge.size() == 2) {
        edges.at(from).at(ValveID{edge}) = 1;
        edge.clear();
      }
    }
  }

  bool changed{true};
  while (changed) {
    changed = false;
    for (UInt e1{0}; e1 < ValveID::max(); ++e1) {
      for (UInt e2{0}; e2 < ValveID::max(); ++e2) {
        if (edges[e1][e2] != 0) {
          for (UInt e3{0}; e3 < ValveID::max(); ++e3) {
            if (edges[e2][e3] == 1 && edges[e1][e3] == 0) {
              edges[e1][e3] = edges[e1][e2] + 1;
              changed = true;
            }
          }
        }
      }
    }
  }

  // Turning default state on.
  for (UInt id{0}; id < ValveID::max(); ++id) {
    edges[id][id] = 0;
  }

  // If rate at a site is zero then change the cost to zero.
  for (UInt e1{0}; e1 < ValveID::max(); ++e1) {
    for (UInt e2{0}; e2 < ValveID::max(); ++e2) {
      if (rates[e2] == 0) {
        edges[e1][e2] = 0;
      }
    }
  }

  std::list<State> current_states;
  std::set<State> visited_states;
  visited_states.insert(State{});
  current_states.push_back(State{});
  UInt best_rate{0};

  for (UInt time_left{26}; time_left > 0; --time_left) {
    for (std::size_t mover{0}; mover < 2; ++mover) {
      std::cout << "Time: " << time_left << ": " << current_states.size()
                << " states to visit. Person " << mover << "\n";

      if (current_states.empty()) {
        break;
      }

      for (auto it{current_states.begin()}; it != current_states.end();) {
        if (it->next_time_[mover] != time_left) {
          ++it;
          continue;
        }

        ValveID from{it->id_[mover]};

        for (UInt to{0}; to < ValveID::max(); ++to) {
          if (edges[from][to] == 0) {
            continue;
          }
          if (edges[from][to] + 1 >= time_left) {
            continue;
          }
          if (it->open_[to]) {
            continue;
          }

          State next_state{*it};
          next_state.id_[mover] = ValveID(to);
          next_state.open_[to] = true;
          auto timestep = edges[from][to] + 1;
          next_state.next_time_[mover] -= timestep;
          next_state.total_rate_ += (next_state.next_time_[mover]) * rates[to];
          best_rate = std::max(best_rate, next_state.total_rate_);

          auto vit = visited_states.find(next_state);
          if (vit != visited_states.end() && vit->total_rate_ >= next_state.total_rate_) {
            continue;
          }
          if (vit != visited_states.end()) {
            visited_states.erase(vit);
          }
          visited_states.insert(next_state);
          current_states.push_back(next_state);
        }
        it = current_states.erase(it);
      }
    }
  }

  std::cout << "Maximum flow rate: " << best_rate << "\n";
  return EXIT_SUCCESS;
}
