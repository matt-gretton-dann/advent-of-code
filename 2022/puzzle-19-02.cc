//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <utility>

using Int = std::int32_t;
using UInt = std::uint32_t;

UInt constexpr ORE{0};
UInt constexpr CLAY{1};
UInt constexpr OBSIDIAN{2};
UInt constexpr GEODE{3};
UInt constexpr resource_count{4};

struct Costs
{
  Costs() noexcept : costs_()
  {
    for (auto& r : costs_) {
      for (auto& c : r) {
        c = 0;
      }
    }
  }
  Costs(Costs const&) noexcept = default;
  Costs(Costs&&) noexcept = default;
  auto operator=(Costs const&) noexcept -> Costs& = default;
  auto operator=(Costs&&) noexcept -> Costs& = default;
  ~Costs() = default;

  std::array<std::array<UInt, resource_count>, resource_count> costs_;
};

struct State
{
  State() : resources_available_({0, 0, 0, 0}), robots_available_({1, 0, 0, 0}) {}

  std::array<uint8_t, resource_count> resources_available_;
  std::array<uint8_t, resource_count> robots_available_;
};

template<typename It1, typename It2>
auto compare_3way(It1 first1, It1 last1, It2 first2, It2 last2) -> std::strong_ordering
{
  for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
    if (*first1 < *first2) {
      return std::strong_ordering::less;
    }
    if (*first1 > *first2) {
      return std::strong_ordering::greater;
    }
  }
  if (first1 == last1) {
    return first2 == last2 ? std::strong_ordering::equal : std::strong_ordering::less;
  }
  return std::strong_ordering::greater;
}

struct StateCompare
{
  auto operator()(State const& lhs, State const& rhs) const noexcept -> bool
  {
    auto order = compare_3way(lhs.resources_available_.begin(), lhs.resources_available_.end(),
                              rhs.resources_available_.begin(), rhs.resources_available_.end());
    if (order != std::strong_ordering::equal) {
      return order == std::strong_ordering::less;
    }
    return std::lexicographical_compare(lhs.robots_available_.begin(), lhs.robots_available_.end(),
                                        rhs.robots_available_.begin(), rhs.robots_available_.end());
  }
};

using StateSet = std::set<State, StateCompare>;

auto generate(Costs const& costs) -> UInt
{
  // Max cost is indexed by resource, and contains the maximum number of resources we need to build
  // any robot.  This provides an upper limit on the maximum number of robots of each type we need
  // to be efficient (as we can only build one robot at a time).
  std::array<UInt, resource_count> max_cost{0, 0, 0, 0};
  for (UInt robot{0}; robot < resource_count; ++robot) {
    for (UInt resource{0}; resource < resource_count; ++resource) {
      max_cost[resource] = std::max(max_cost[resource], costs.costs_[robot][resource]);
    }
  }

  // However as many GEODE robots as possible should be built
  max_cost[GEODE] = std::numeric_limits<UInt>::max();

  constexpr UInt total_time{32};  // Time to run for
  // Use a set for the states to examine to ensure we remove duplicates.
  StateSet next_states;  // The states to examine next.
  next_states.insert(State{});

  for (UInt t{0}; t < total_time; ++t) {
    std::cout << "Time " << t << ": " << next_states.size() << " states to consider.\n";
    StateSet const current_states{std::move(next_states)};
    next_states = StateSet{};

    for (auto const& state : current_states) {
      // Let's build a robot:
      auto built{0};
      auto robots{0};
      for (UInt robot{0}; robot < resource_count; ++robot) {
        if (state.robots_available_[robot] >= max_cost[robot]) {
          continue;
        }
        if (state.robots_available_[robot] != 0) {
          ++robots;
        }
        bool build{true};
        for (UInt resource{0}; resource < resource_count; ++resource) {
          if (costs.costs_[robot][resource] > state.resources_available_[resource]) {
            build = false;
          }
        }
        if (build) {
          ++built;
          State new_state{state};
          for (UInt resource{0}; resource < resource_count; ++resource) {
            new_state.resources_available_[resource] -= costs.costs_[robot][resource];
            new_state.resources_available_[resource] += state.robots_available_[resource];
          }
          new_state.robots_available_[robot] += 1;
          next_states.insert(new_state);
        }
      }

      if (built != robots) {
        // Now build a robot that just collects:
        State new_state{state};
        for (UInt resource{0}; resource < resource_count; ++resource) {
          new_state.resources_available_[resource] += state.robots_available_[resource];
        }
        next_states.insert(new_state);
      }
    }
  }

  auto const& max_elt =
    *std::max_element(next_states.begin(), next_states.end(), [](auto const& l, auto const& r) {
      return l.resources_available_[GEODE] < r.resources_available_[GEODE];
    });

  return max_elt.resources_available_[GEODE];
}

auto main() -> int
{
  std::string line;
  std::map<UInt, Costs> cost_map;
  std::regex const re{"Blueprint (\\d+): Each ore robot costs (\\d+) ore. Each clay robot costs "
                      "(\\d+) ore. Each obsidian robot costs (\\d+) ore and (\\d+) clay. Each "
                      "geode robot costs (\\d+) ore and (\\d+) obsidian."};

  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Cannot interpret: " << line << "\n";
      return EXIT_FAILURE;
    }

    UInt const id{static_cast<UInt>(std::stoul(m.str(1)))};
    Costs costs;
    costs.costs_[ORE][ORE] = static_cast<UInt>(std::stoul(m.str(2)));
    costs.costs_[CLAY][ORE] = static_cast<UInt>(std::stoul(m.str(3)));
    costs.costs_[OBSIDIAN][ORE] = static_cast<UInt>(std::stoul(m.str(4)));
    costs.costs_[OBSIDIAN][CLAY] = static_cast<UInt>(std::stoul(m.str(5)));
    costs.costs_[GEODE][ORE] = static_cast<UInt>(std::stoul(m.str(6)));
    costs.costs_[GEODE][OBSIDIAN] = static_cast<UInt>(std::stoul(m.str(7)));

    cost_map.insert({id, costs});
  }

  UInt geode_count{1};
  for (UInt i{1}; i <= 3; ++i) {
    auto costs{cost_map.find(i)};
    auto result{generate(costs->second)};
    std::cout << "For ID " << costs->first << " result is: " << result << "\n";
    geode_count *= result;
  }

  std::cout << "Total quality: " << geode_count << "\n";
  return EXIT_SUCCESS;
}
