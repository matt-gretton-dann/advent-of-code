//
// Created by Matthew Gretton-Dann on 09/12/2022.
//

#include <array>
#include <iostream>
#include <list>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

enum class Op { undef, add, mult, dbl, square };

struct Monkey
{
  auto add_item(UInt score) { items_.push_back(score); }
  auto op(Op op)
  {
    assert(op == Op::dbl || op == Op::square);
    op_ = op;
  }
  auto op(Op op, UInt amt)
  {
    assert(op == Op::add || op == Op::mult);
    op_ = op;
    amt_ = amt;
  }
  auto test(UInt test) { test_ = test; }
  auto true_monkey(UInt monkey) { true_monkey_ = monkey; }
  auto false_monkey(UInt monkey) { false_monkey_ = monkey; }

  [[nodiscard]] auto process_front() -> std::pair<UInt, UInt>
  {
    auto item{items_.front()};
    items_.pop_front();
    switch (op_) {
    case Op::square:
      item *= item;
      break;
    case Op::dbl:
      item += item;
      break;
    case Op::add:
      item += amt_;
      break;
    case Op::mult:
      item *= amt_;
      break;
    default:
      std::cerr << "Unexpected op\n";
      std::abort();
    }
    item /= 3;
    UInt const monkey{(item % test_ == 0) ? true_monkey_ : false_monkey_};
    ++inspection_count_;
    return std::make_pair(item, monkey);
  }

  [[nodiscard]] auto empty() const -> bool { return items_.empty(); }

  [[nodiscard]] auto score() const -> UInt { return inspection_count_; }

private:
  std::list<UInt> items_;
  Op op_{Op::undef};
  UInt amt_{0};
  UInt test_{0};
  UInt true_monkey_{std::numeric_limits<Int>::max()};
  UInt false_monkey_{std::numeric_limits<Int>::max()};
  UInt inspection_count_{0};
};

struct State
{
  auto populate(std::istream& is) -> bool
  {
    using namespace std::literals::string_literals;

    std::string line;
    auto it{monkies_.end()};
    while (std::getline(is, line)) {
      if (line.empty()) {
        continue;
      }
      if (line.substr(0, 7) == "Monkey "s) {
        it = monkies_.insert(monkies_.end(), Monkey{});
      }
      else if (line.substr(0, 18) == "  Starting items: "s) {
        assert(it != monkies_.end());
        std::size_t pos{18};
        while (pos < line.size()) {
          if (line[pos] == ' ' || line[pos] == ',') {
            ++pos;
          }
          else if (std::isdigit(line[pos]) != 0) {
            std::size_t len{0};
            it->add_item(std::stoull(line.substr(pos), &len));
            pos += len;
          }
          else {
            std::cerr << "Unable to interpret: " << line << '\n';
            return false;
          }
        }
      }
      else if (line.substr(0, 23) == "  Operation: new = old "s) {
        assert(it != monkies_.end());

        if (line[23] == '+') {
          if (line.substr(25) == "old") {
            it->op(Op::dbl);
          }
          else {
            it->op(Op::add, std::stoull(line.substr(25)));
          }
        }
        else {
          if (line[23] != '*') {
            std::cerr << "Unable to interpret: " << line << '\n';
            return false;
          }
          if (line.substr(25) == "old") {
            it->op(Op::square);
          }
          else {
            it->op(Op::mult, std::stoull(line.substr(25)));
          }
        }
      }
      else if (line.substr(0, 21) == "  Test: divisible by "s) {
        assert(it != monkies_.end());

        it->test(std::stoull(line.substr(21)));
      }
      else if (line.substr(0, 29) == "    If true: throw to monkey "s) {
        assert(it != monkies_.end());

        it->true_monkey(std::stoull(line.substr(29)));
      }
      else if (line.substr(0, 30) == "    If false: throw to monkey "s) {
        assert(it != monkies_.end());

        it->false_monkey(std::stoull(line.substr(30)));
      }
      else {
        std::cerr << "Unable to interpret: " << line << '\n';
        return false;
      }
    }
    return true;
  }

  auto run_round()
  {
    unsigned m{0};
    for (auto& monkey : monkies_) {
      std::cout << "Monkey: " << m++ << '\n';
      while (!monkey.empty()) {
        auto [item, dest] = monkey.process_front();
        std::cout << "  Moving " << item << " to " << dest << '\n';
        monkies_[dest].add_item(item);
      }
    }
  }

  [[nodiscard]] auto score() const -> UInt
  {
    UInt max1{0};
    UInt max2{0};
    for (auto const& monkey : monkies_) {
      if (monkey.score() >= max1) {
        max2 = max1;
        max1 = monkey.score();
      }
      else if (monkey.score() >= max2) {
        max2 = monkey.score();
      }
    }

    std::cout << "Max score = " << max1 << " *  " << max2 << " = " << max1 * max2 << "\n";
    return max1 * max2;
  }

  std::vector<Monkey> monkies_;
};

auto main() -> int
{
  State state;

  if (!state.populate(std::cin)) {
    return EXIT_FAILURE;
  }
  for (UInt i = 0; i < 20; ++i) {
    state.run_round();
  }
  std::cout << "Score: " << state.score() << '\n';
  return 0;
}
