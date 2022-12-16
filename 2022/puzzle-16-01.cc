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
  ValveID current_{"AA"s};
  UInt total_rate_{0};
  std::vector<UInt> open_{std::vector<UInt>(ValveID::max(), false)};

  auto operator<=>(State const& rhs) const noexcept -> std::strong_ordering
  {
    if (current_ != rhs.current_) {
      return current_ <=> rhs.current_;
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
  std::vector<std::list<ValveID>> edges(ValveID::max(), std::list<ValveID>{});
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
        auto& l{edges.at(from)};
        auto id{ValveID{edge}};
        l.push_back(id);
        ++size;
        edge.clear();
      }
    }
  }

  std::list<State> next_states;
  std::set<State> visited_states;
  visited_states.insert(State{});
  next_states.push_back(State{});
  UInt best_rate{0};

  for (UInt time_left{30}; time_left > 0; --time_left) {
    std::list<State> const current_states{std::move(next_states)};
    std::cout << "Time: " << time_left << ": " << current_states.size() << " states to visit.\n";

    if (current_states.empty()) {
      break;
    }

    next_states.clear();
    for (auto const& state : current_states) {
      if (!state.open_.at(state.current_) && rates.at(state.current_) != 0) {
        State next_state{state};
        next_state.open_.at(state.current_) = true;
        next_state.total_rate_ += rates.at(state.current_) * (time_left - 1);
        best_rate = std::max(best_rate, next_state.total_rate_);
        next_states.push_back(next_state);
        visited_states.erase(next_state);
        visited_states.insert(next_state);
      }

      for (auto e : edges[state.current_]) {
        State next_state{state};
        next_state.current_ = e;
        auto vit = visited_states.find(next_state);
        if (vit != visited_states.end() && vit->total_rate_ >= next_state.total_rate_) {
          continue;
        }
        if (vit != visited_states.end()) {
          visited_states.erase(vit);
        }
        next_states.push_back(next_state);
        visited_states.insert(next_state);
      }
    }
  }

  std::cout << "Maximum flow rate: " << best_rate << "\n";
  return EXIT_SUCCESS;
}
