//
// Created by Matthew Gretton-Dann on 05/12/2021.
//

#include <algorithm>
#include <cassert>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

struct Bot
{
  void receive(unsigned long v)  // NOLINT(google-runtime-int)
  {
    assert(!is_full());
    values_.emplace_back(v);
  }

  void clear() { values_.clear(); }

  [[nodiscard]] auto is_full() const noexcept -> bool { return values_.size() == 2; }

  [[nodiscard]] auto low_value() const noexcept -> unsigned
  {
    assert(is_full());
    return *std::min_element(values_.begin(), values_.end());
  }

  [[nodiscard]] auto high_value() const noexcept -> unsigned
  {
    assert(is_full());
    return *std::max_element(values_.begin(), values_.end());
  }

private:
  std::vector<unsigned> values_;
};

enum class Destination { bot, output };

auto to_destination(std::string const& s) -> Destination
{
  if (s == "bot") {
    return Destination::bot;
  }
  if (s == "output") {
    return Destination::output;
  }
  abort();
}

struct Action
{
  unsigned long id_;
  Destination low_dest_type_;
  unsigned long low_dest_id_;
  Destination high_dest_type_;
  unsigned long high_dest_id_;
};

auto main() -> int
{
  static std::regex value_action{"value (\\d+) goes to bot (\\d+)"};
  static std::regex bot_action{R"(bot (\d+) gives low to (.+) (\d+) and high to (.+) (\d+))"};
  std::vector<Bot> bots;
  std::vector<unsigned> outputs;
  std::vector<Action> actions;
  std::string line;
  while (std::getline(std::cin, line)) {
    std::smatch m;
    if (std::regex_search(line, m, value_action)) {
      auto value{std::stoul(m.str(1))};
      auto bot{std::stoul(m.str(2))};
      if (bot >= bots.size()) {
        bots.resize(bot + 1);
      }
      bots.at(bot).receive(value);
    }
    else if (std::regex_search(line, m, bot_action)) {
      auto bot{std::stoul(m.str(1))};
      auto low_type{to_destination(m.str(2))};
      auto low_id{std::stoul(m.str(3))};
      auto high_type{to_destination(m.str(4))};
      auto high_id{std::stoul(m.str(5))};
      if (bot >= bots.size()) {
        bots.resize(bot + 1);
      }
      if (low_type == Destination::output && low_id >= outputs.size()) {
        outputs.resize(low_id + 1);
      }
      if (high_type == Destination::output && high_id >= outputs.size()) {
        outputs.resize(high_id + 1);
      }
      actions.push_back(Action{bot, low_type, low_id, high_type, high_id});
    }
    else {
      std::cerr << "Unable to interpret: " << line << '\n';
      return 1;
    }
  }

  for (std::size_t i{0}; i < outputs.size(); ++i) {
    outputs[i] = 0;
  }

  while (outputs[0] == 0 || outputs[1] == 0 || outputs[2] == 0) {
    for (auto action : actions) {
      auto& bot{bots.at(action.id_)};
      if (!bot.is_full()) {
        continue;
      }

      if (action.low_dest_type_ == Destination::bot) {
        bots.at(action.low_dest_id_).receive(bot.low_value());
      }
      else {
        outputs.at(action.low_dest_id_) = bot.low_value();
      }
      if (action.high_dest_type_ == Destination::bot) {
        bots.at(action.high_dest_id_).receive(bot.high_value());
      }
      else {
        outputs.at(action.high_dest_id_) = bot.high_value();
      }
      bot.clear();
    }
  }

  std::cout << "Result: " << outputs[0] * outputs[1] * outputs[2] << '\n';
}
