#include <array>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <unordered_set>

using Int = long;
using StateName = char;

struct SubInstruction
{
  bool write_;
  Int move_delta_;
  StateName next_state_;
};

struct Instruction
{
  [[nodiscard]] auto write(bool value) const noexcept -> bool
  {
    return subinstructions_[value ? 1 : 0].write_;
  }

  [[nodiscard]] auto move_delta(bool value) const noexcept -> Int
  {
    return subinstructions_[value ? 1 : 0].move_delta_;
  }

  [[nodiscard]] auto next_state(bool value) const noexcept -> StateName
  {
    return subinstructions_[value ? 1 : 0].next_state_;
  }

  std::array<SubInstruction, 2> subinstructions_;
};

auto main() -> int
{
  std::map<StateName, Instruction> state_machine;
  std::regex begin_state_re{"Begin in state (.)."};
  std::regex step_count_re{"Perform a diagnostic checksum after (\\d+) steps."};
  std::regex in_state_re{"In state (.):"};
  std::regex if_current_value_re{"  If the current value is (.):"};
  std::regex write_re{"    - Write the value (.)."};
  std::regex move_re{"    - Move one slot to the (left|right)."};
  std::regex next_state_re{"    - Continue with state (.)."};

  std::string line;
  StateName current_state{'\0'};
  unsigned long step_count{0};
  unsigned current_value{2};

  auto it{state_machine.end()};
  while (std::getline(std::cin, line) && line != "END") {
    std::smatch m;
    if (std::regex_search(line, m, begin_state_re)) {
      assert(current_state == '\0');
      current_state = m.str(1)[0];
    }
    else if (std::regex_search(line, m, step_count_re)) {
      assert(step_count == 0);
      step_count = std::stoul(m.str(1));
    }
    else if (std::regex_search(line, m, in_state_re)) {
      auto [it2, success] = state_machine.insert({m.str(1)[0], {}});
      assert(success);
      it = it2;
      current_value = 2;
    }
    else if (std::regex_search(line, m, if_current_value_re)) {
      assert(it != state_machine.end());
      current_value = m.str(1)[0] - '0';
      assert(current_value == 0 || current_value == 1);
    }
    else if (std::regex_search(line, m, write_re)) {
      assert(it != state_machine.end());
      assert(current_value != 2);
      it->second.subinstructions_[current_value].write_ = m.str(1) == "1";
    }
    else if (std::regex_search(line, m, move_re)) {
      assert(it != state_machine.end());
      assert(current_value != 2);
      it->second.subinstructions_[current_value].move_delta_ = m.str(1) == "left" ? -1 : 1;
    }
    else if (std::regex_search(line, m, next_state_re)) {
      assert(it != state_machine.end());
      assert(current_value != 2);
      it->second.subinstructions_[current_value].next_state_ = m.str(1)[0];
    }
  }

  std::unordered_set<Int> marked;
  Int pos{0};
  for (unsigned long i{0}; i < step_count; ++i) {
    auto it{state_machine.find(current_state)};
    assert(it != state_machine.end());
    auto const& subinstruction = it->second.subinstructions_[marked.contains(pos) ? 1 : 0];
    if (subinstruction.write_) {
      marked.insert(pos);
    }
    else {
      marked.erase(pos);
    }
    pos += subinstruction.move_delta_;
    current_state = subinstruction.next_state_;
  }

  std::cout << "Number marked: " << marked.size() << '\n';
}
