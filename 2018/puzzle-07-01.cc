//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <utility>

struct Step
{
  explicit Step(std::string const& dependencies) : done_(false), dependencies_(dependencies) {}

  bool done_;
  std::string dependencies_;
};

using UInt = std::int64_t;
using StepMap = std::map<char, Step>;

auto main() -> int
{
  std::string line;
  StepMap step_map;
  std::regex const re{"Step ([A-Z]) must be finished before step ([A-Z]) can begin."};

  // Read data
  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (!std::regex_match(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << "\n";
      return EXIT_FAILURE;
    }
    char id{m.str(2)[0]};
    auto [it, success] = step_map.insert({id, Step(m.str(1))});
    if (!success) {
      it->second.dependencies_ += m.str(1);
    }

    // Ensure we have every id we've seen present in the map even if it has no dependencies
    step_map.insert({m.str(1)[0], Step("")});
  }

  std::string order;
  while (!std::all_of(step_map.begin(), step_map.end(),
                      [](auto const& step) { return step.second.done_; })) {
    char to_do{'Z' + 1};
    for (auto& step : step_map) {
      if (step.second.done_) {
        continue;
      }
      auto const& dependencies{step.second.dependencies_};
      if (std::all_of(dependencies.begin(), dependencies.end(),
                      [&step_map](auto c) { return step_map.find(c)->second.done_; })) {
        to_do = std::min(step.first, to_do);
      }
    }
    order += to_do;
    step_map.find(to_do)->second.done_ = true;
  }

  std::cout << "Order: " << order << "\n";
  return EXIT_SUCCESS;
}
