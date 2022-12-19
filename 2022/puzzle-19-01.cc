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
using Point3 = std::tuple<UInt, UInt, UInt>;

using namespace std::string_literals;

enum Resources { Ore, Clay, Obsidian, Geode };
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

  std::array<UInt, resource_count> resources_available_;
  std::array<UInt, resource_count> robots_available_;
};

auto generate(Costs const& costs) -> UInt
{
  UInt max_geode_count{0};
  constexpr UInt total_time{24};
  std::list<State> next_states{State()};

  for (UInt t{0}; t < total_time; ++t) {
    std::cout << "  Time " << t << ": " << next_states.size()
              << " States to consider.  Max geode: " << max_geode_count << "\n";
    std::list<State> const current_states{std::move(next_states)};
    next_states = std::list<State>();

    for (auto const& state : current_states) {
      // See if this is the best state we've seen:
      max_geode_count = std::max(max_geode_count, state.resources_available_[GEODE]);

      // Let's build a robot:
      for (UInt robot{0}; robot < resource_count; ++robot) {
        bool build{true};
        for (UInt resource{0}; resource < resource_count; ++resource) {
          if (costs.costs_[robot][resource] > state.resources_available_[resource]) {
            build = false;
          }
        }
        if (build) {
          State new_state{state};
          for (UInt resource{0}; resource < resource_count; ++resource) {
            new_state.resources_available_[resource] -= costs.costs_[robot][resource];
            new_state.resources_available_[resource] += state.robots_available_[resource];
          }
          new_state.robots_available_[robot] += 1;
          next_states.push_back(new_state);
        }
      }

      // Now build a robot that just collects:
      State new_state{state};
      for (UInt resource{0}; resource < resource_count; ++resource) {
        new_state.resources_available_[resource] += state.robots_available_[resource];
      }
      next_states.push_back(new_state);
    }
  }
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

    UInt id{std::stoull(m.str(1))};
    Costs costs;
    costs.costs_[ORE][ORE] = std::stoull(m.str(2));
    costs.costs_[CLAY][ORE] = std::stoull(m.str(3));
    costs.costs_[OBSIDIAN][ORE] = std::stoull(m.str(4));
    costs.costs_[OBSIDIAN][CLAY] = std::stoull(m.str(5));
    costs.costs_[GEODE][ORE] = std::stoull(m.str(6));
    costs.costs_[GEODE][OBSIDIAN] = std::stoull(m.str(7));

    cost_map.insert({id, costs});
  }

  UInt total_quality{0};
  for (auto const& costs : cost_map) {
    auto result{generate(costs.second)};
    std::cout << "For ID " << costs.first << " result is: " << result << "\n";
    total_quality += costs.first * result;
  }

  std::cout << "Total quality: " << total_quality << "\n";
  return EXIT_SUCCESS;
}
