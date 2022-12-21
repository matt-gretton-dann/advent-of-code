//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <algorithm>
#include <array>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <utility>

using Int = std::int64_t;

struct Step
{
  explicit Step(std::string dependencies) : dependencies_(std::move(dependencies)) {}

  [[nodiscard]] auto done_by(Int time) const noexcept -> bool
  {
    return done_time_ != 0 && done_time_ <= time;
  }

  Int done_time_{0};
  std::string dependencies_;
};

constexpr Int worker_count{5};
constexpr Int min_time{61};

using StepMap = std::map<char, Step>;
using Workers = std::array<Int, worker_count>;

auto dependencies_done(std::string const& dependencies, StepMap const& step_map, Int time) -> bool
{
  return std::all_of(dependencies.begin(), dependencies.end(),
                     [&step_map, time](auto c) { return step_map.find(c)->second.done_by(time); });
}

auto find_available_step(StepMap const& step_map, Int time) -> char
{
  for (auto const& step : step_map) {
    if (step.second.done_time_ != 0) {
      continue;
    }
    auto const& dependencies{step.second.dependencies_};
    if (dependencies_done(dependencies, step_map, time)) {
      return step.first;
    }
  }
  return 'Z' + 1;
}

auto process_time_step(StepMap& step_map, Workers& workers, Int time)
{
  std::cout << std::setw(4) << time << ": ";
  for (auto& w : workers) {
    if (time < w) {
      std::cout << "| ";
      continue;
    }
    char const to_do{find_available_step(step_map, time)};
    if (to_do != 'Z' + 1) {
      std::cout << to_do << " ";
      w = time + min_time + (to_do - 'A');
      step_map.find(to_do)->second.done_time_ = w;
    }
    else {
      std::cout << ". ";
    }
  }
  std::cout << "\n";
}

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
    char const id{m.str(2)[0]};
    auto [it, success] = step_map.insert({id, Step(m.str(1))});
    if (!success) {
      it->second.dependencies_ += m.str(1);
    }

    // Ensure we have every id we've seen present in the map even if it has no dependencies
    step_map.insert({m.str(1)[0], Step("")});
  }

  Workers workers{0};

  Int time{0};
  for (; !std::all_of(step_map.begin(), step_map.end(),
                      [time](auto const& step) { return step.second.done_by(time); });
       ++time) {
    process_time_step(step_map, workers, time);
  }

  std::cout << "Time taken: " << time << "\n";
  return EXIT_SUCCESS;
}
