//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <cassert>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Ints = std::vector<std::pair<Int, Int>>;

using namespace std::string_literals;

struct Monkey
{
  Monkey() = default;

  explicit Monkey(Int value)
    : value_(value), value_known_(true)
  {
  }

  Monkey(std::string lhs, char op, std::string rhs)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op)
  {
  }

  Monkey(Monkey const&) = default;
  Monkey(Monkey&&) = default;
  auto operator=(Monkey const&) -> Monkey& = default;
  auto operator=(Monkey&&) -> Monkey& = default;
  ~Monkey() = default;

  Int value_{0};
  bool value_known_{false};
  std::string lhs_;
  std::string rhs_;
  char op_{'\0'};
};

auto calculate(Int lhs, char op, Int rhs)
{
  switch (op) {
  case '+':
    return lhs + rhs;
  case '-':
    return lhs - rhs;
  case '*':
    return lhs * rhs;
  case '/':
    return lhs / rhs;
  default:
    std::abort();
  }
}

auto main() -> int
{
  std::string line;
  std::map<std::string, Monkey> monkies;

  while (std::getline(std::cin, line)) {
    std::string const id(line.substr(0, 4));
    if (std::isdigit(line[6]) != 0) {
      auto [it, success] = monkies.insert({id, Monkey(std::stoll(line.substr(6)))});
      assert(success);
    }
    else {
      auto [it, success] =
        monkies.insert({id, Monkey(line.substr(6, 4), line[11], line.substr(13))});
      assert(success);
    }
  }

  while (!monkies["root"s].value_known_) {
    for (auto& monkey : monkies) {
      if (monkey.second.value_known_) {
        continue;
      }

      auto lhs = monkies.find(monkey.second.lhs_);
      auto rhs = monkies.find(monkey.second.rhs_);
      if (!lhs->second.value_known_ || !rhs->second.value_known_) {
        continue;
      }

      monkey.second.value_ = calculate(lhs->second.value_, monkey.second.op_, rhs->second.value_);
      monkey.second.value_known_ = true;
    }
  }

  std::cout << "Root's value: " << monkies["root"s].value_ << "\n";
  return EXIT_SUCCESS;
}
