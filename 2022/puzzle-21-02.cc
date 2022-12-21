//
// Created by Matthew Gretton-Dann on 16/12/2022.
//

#include <array>
#include <iostream>
#include <map>
#include <numeric>
#include <stdexcept>
#include <utility>

using Int = std::int64_t;
using UInt = std::uint64_t;
using Ints = std::vector<std::pair<Int, Int>>;

using namespace std::string_literals;

struct Monkey
{
  Monkey() = default;

  explicit Monkey(Int value) : value_(value), value_known_(true) {}

  Monkey(std::string lhs, char op, std::string rhs)
      : lhs_(std::move(lhs)), rhs_(std::move(rhs)), op_(op)
  {
  }

  static auto human() -> Monkey
  {
    Monkey monkey;
    monkey.value_known_ = false;
    monkey.op_ = 'h';
    return monkey;
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
  case '=':
    return static_cast<Int>(lhs == rhs ? 1 : 0);
  default:
    std::abort();
  }
}

auto find_lhs(Int result, char op, Int rhs)
{
  switch (op) {
  case '+':  // result = lhs + rhs -> lhs = result - rhs;
    return result - rhs;
  case '-':  // result = lhs - rhs -> lhs = result + rhs;
    return result + rhs;
  case '*':  // result = lhs * rhs -> lhs = result / rhs
    return result / rhs;
  case '/':  // result = lhs / rhs -> lhs = result * rhs;
    return result * rhs;
  case '=':
    return rhs;
  default:
    std::abort();
  }
}

auto find_rhs(Int result, Int lhs, char op)
{
  switch (op) {
  case '+':  // result = lhs + rhs -> rhs = result - lhs;
    return result - lhs;
  case '-':  // result = lhs - rhs -> rhs = lhs - result;
    return lhs - result;
  case '*':  // result = lhs * rhs -> rhs = result / lhs
    return result / lhs;
  case '/':  // result = lhs / rhs -> rhs = lhs / result;
    return lhs / result;
  case '=':
    return lhs;
  default:
    std::abort();
  }
}

auto calculate_up(std::map<std::string, Monkey>& monkies)
{
  bool changed{true};
  while (changed) {
    changed = false;
    for (auto& monkey : monkies) {
      if (monkey.second.value_known_) {
        continue;
      }

      auto lhs = monkies.find(monkey.second.lhs_);
      auto rhs = monkies.find(monkey.second.rhs_);
      if (!lhs->second.value_known_ || !rhs->second.value_known_) {
        continue;
      }
      if (lhs->second.op_ == 'h' || rhs->second.op_ == 'h') {
        continue;
      }

      monkey.second.value_ = calculate(lhs->second.value_, monkey.second.op_, rhs->second.value_);
      monkey.second.value_known_ = true;
      changed = true;
    }
  }
}

auto find_human_value(std::map<std::string, Monkey>& monkies)
{
  auto monkey{monkies.find("root"s)};
  monkey->second.value_ = 1;
  monkey->second.value_known_ = true;
  while (monkey->first != "humn"s) {
    auto lhs{monkies.find(monkey->second.lhs_)};
    auto rhs{monkies.find(monkey->second.rhs_)};
    assert(lhs->second.value_known_ ^ rhs->second.value_known_);
    if (!lhs->second.value_known_) {
      lhs->second.value_ = find_lhs(monkey->second.value_, monkey->second.op_, rhs->second.value_);
      lhs->second.value_known_ = true;
      monkey = lhs;
    }
    if (!rhs->second.value_known_) {
      rhs->second.value_ = find_rhs(monkey->second.value_, lhs->second.value_, monkey->second.op_);
      rhs->second.value_known_ = true;
      monkey = rhs;
    }
  }
}

auto main() -> int
{
  std::string line;
  std::map<std::string, Monkey> monkies;

  while (std::getline(std::cin, line)) {
    std::string const id(line.substr(0, 4));
    if (std::isdigit(line[6]) != 0) {
      if (id == "humn"s) {
        auto [it, success] = monkies.insert({id, Monkey::human()});
        assert(success);
      }
      else {
        auto [it, success] = monkies.insert({id, Monkey(std::stoll(line.substr(6)))});
        assert(success);
      }
    }
    else {
      char const op{(id == "root"s) ? '=' : line[11]};
      auto [it, success] = monkies.insert({id, Monkey(line.substr(6, 4), op, line.substr(13))});
      assert(success);
    }
  }

  calculate_up(monkies);
  find_human_value(monkies);

  std::cout << "Human's value: " << monkies["humn"s].value_ << "\n";
  return EXIT_SUCCESS;
}
