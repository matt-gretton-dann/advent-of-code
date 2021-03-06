#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <variant>
#include <vector>

using Int = long;
using Register = char;
using Operand = std::variant<Register, Int, std::monostate>;
enum class Opcode { set, sub, mul, jnz };

auto to_opcode(std::string const& s) -> Opcode
{
  if (s == "set") {
    return Opcode::set;
  }
  if (s == "sub") {
    return Opcode::sub;
  }
  if (s == "mul") {
    return Opcode::mul;
  }
  if (s == "jnz") {
    return Opcode::jnz;
  }
  abort();
}

static auto operand(std::string const& s, std::size_t idx) -> std::pair<Operand, std::size_t>
{
  if (idx >= s.size()) {
    return {std::monostate{}, idx};
  }
  if (s[idx] >= 'a' && s[idx] <= 'z') {
    return {s[idx], idx + 1};
  }

  std::size_t end{0};
  auto v{std::stol(s.substr(idx), &end)};
  return {v, idx + end};
}

struct Instruction
{
  auto opcode() const noexcept -> Opcode { return opcode_; }
  auto op1() const noexcept -> Operand { return op1_; }
  auto op2() const noexcept -> Operand { return op2_; }

  static auto instruction(std::string const& s) -> Instruction
  {
    auto opcode{to_opcode(s.substr(0, 3))};
    auto [op1, idx] = operand(s, 4);
    auto [op2, idx2] = operand(s, idx + 1);
    return {opcode, op1, op2};
  }

private:
  Instruction(Opcode opcode, Operand op1, Operand op2) : opcode_(opcode), op1_(op1), op2_(op2) {}

  Opcode opcode_;
  Operand op1_;
  Operand op2_;
};

using Instructions = std::vector<Instruction>;

struct State
{
  explicit State(Instructions const& instructions) : instructions_(instructions) {}

  void execute()
  {
    while (pc_ >= 0 && pc_ < instructions_.size()) {
      auto const& instruction{instructions_[pc_]};
      switch (instruction.opcode()) {
      case Opcode::sub:
        set(instruction.op1(), value(instruction.op1()) - value(instruction.op2()));
        break;
      case Opcode::jnz:
        if (value(instruction.op1()) != 0) {
          pc_ += value(instruction.op2()) - 1;
        }
        break;
      case Opcode::mul:
        set(instruction.op1(), value(instruction.op1()) * value(instruction.op2()));
        ++mul_count_;
        break;
      case Opcode::set:
        set(instruction.op1(), value(instruction.op2()));
        break;
      default:
        abort();
      }
      ++pc_;
    }

    std::cout << "Mul executed: " << mul_count_ << " times.\n";
  }

  void set(Register r, Int value) { registers_.insert_or_assign(r, value); }

private:
  void set(Operand const& op, Int value)
  {
    if (std::holds_alternative<Register>(op)) {
      registers_.insert_or_assign(std::get<Register>(op), value);
    }
    else {
      abort();
    }
  }

  auto value(Operand const& op) const -> Int
  {
    if (std::holds_alternative<Register>(op)) {
      auto it{registers_.find(std::get<Register>(op))};
      return it == registers_.end() ? 0 : it->second;
    }
    if (std::holds_alternative<Int>(op)) {
      return std::get<Int>(op);
    }
    abort();
  }

  Instructions instructions_;
  std::map<Register, Int> registers_;
  Int pc_{0};
  Int mul_count_{0};
};

auto main() -> int
{
  Instructions instructions;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    instructions.push_back(Instruction::instruction(line));
  }

  State cpu{instructions};
  cpu.execute();
  return 0;
}
