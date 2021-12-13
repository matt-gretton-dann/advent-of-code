//
// Created by Matthew Gretton-Dann on 12/12/2021.
//

#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

enum class Register { a, b, c, d };
auto to_register(char c) -> Register
{
  switch (c) {
  case 'a':
    return Register::a;
  case 'b':
    return Register::b;
  case 'c':
    return Register::c;
  case 'd':
    return Register::d;
  default:
    abort();
  }
}

enum class Opcode { cpy, inc, dec, jnz, tgl };

auto to_opcode(std::string const& s) -> Opcode
{
  if (s == "cpy") {
    return Opcode::cpy;
  }
  if (s == "inc") {
    return Opcode::inc;
  }
  if (s == "dec") {
    return Opcode::dec;
  }
  if (s == "jnz") {
    return Opcode::jnz;
  }
  if (s == "tgl") {
    return Opcode::tgl;
  }
  abort();
}

using Int = long;  // NOLINT(google-runtime-int)
using Operand = std::variant<Register, Int, std::monostate>;

struct Instruction
{
  Instruction(Opcode opcode, Operand const& op1, Operand const& op2)
      : opcode_(opcode), op1_(op1), op2_(op2)
  {
  }

  [[nodiscard]] auto opcode() const noexcept -> Opcode { return opcode_; }
  [[nodiscard]] auto op1() const noexcept -> Operand const& { return op1_; }
  [[nodiscard]] auto op2() const noexcept -> Operand const& { return op2_; }

  void toggle()
  {
    switch (opcode_) {
    case Opcode::cpy:
      opcode_ = Opcode::jnz;
      break;
    case Opcode::jnz:
      opcode_ = Opcode::cpy;
      break;
    case Opcode::inc:
      opcode_ = Opcode::dec;
      break;
    case Opcode::dec:
    case Opcode::tgl:
      opcode_ = Opcode::inc;
      break;
    }
  }

  static auto instruction(std::string const& s) -> Instruction
  {
    Opcode opcode = to_opcode(s.substr(0, 3));
    Operand op1{std::monostate()};
    Operand op2{std::monostate()};
    std::size_t idx{4};

    {
      char* end{nullptr};
      auto val{
        std::strtol(s.data() + idx,  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
                    &end, 10)};
      if (end != s.data() + idx) {  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        op1 = val;
        idx = end - s.data();
      }
      else {
        op1 = to_register(s[idx]);
        idx += 1;
      }
    }

    if (idx < s.size()) {
      assert(s[idx] == ' ');
      ++idx;
      char* end{nullptr};
      auto val{
        std::strtol(s.data() + idx,  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                    &end, 10)};
      if (end != s.data() + idx) {  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        op2 = val;
      }
      else {
        op2 = to_register(s[idx]);
      }
    }

    return {opcode, op1, op2};
  }

private:
  Opcode opcode_;
  Operand op1_;
  Operand op2_;
};

struct State
{
  explicit State(std::vector<Instruction> instructions) : instructions_(std::move(instructions)) {}

  [[nodiscard]] auto reg(Register r) const noexcept -> Int
  {
    return registers_
      [static_cast<unsigned>(  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        r)];
  }
  void reg(Register r, Int v) noexcept
  {
    registers_[static_cast<unsigned>(  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
      r)] = v;
  }

  void execute()
  {
    while (pc_ < instructions_.size()) {
      auto& i{instructions_[pc_]};
      switch (i.opcode()) {
      case Opcode::cpy:
        set(i.op2(), value(i.op1()));
        break;
      case Opcode::inc:
        set(i.op1(), value(i.op1()) + 1);
        break;
      case Opcode::dec:
        set(i.op1(), value(i.op1()) - 1);
        break;
      case Opcode::jnz:
        if (value(i.op1()) != 0) {
          pc_ += value(i.op2()) - 1;
        }
        break;
      case Opcode::tgl:
        auto dest{pc_ + value(i.op1())};
        if (dest >= 0 && dest < instructions_.size()) {
          instructions_[dest].toggle();
        }
        break;
      }

      ++pc_;
    }
  }

private:
  auto value(Operand const& o) -> Int
  {
    if (std::holds_alternative<Register>(o)) {
      return registers_
        [static_cast<unsigned>(  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
          std::get<Register>(o))];
    }
    if (std::holds_alternative<Int>(o)) {
      return std::get<Int>(o);
    }
    abort();
  }

  void set(Operand const& dest, Int value)
  {
    if (std::holds_alternative<Register>(dest)) {
      registers_[  // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
        static_cast<unsigned>(std::get<Register>(dest))] = value;
    }
  }

  std::vector<Instruction> instructions_;
  std::array<Int, 4> registers_{0, 0, 0, 0};
  Int pc_{0};
};

auto main() -> int
{
  std::vector<Instruction> instructions;
  std::string line;
  while (std::getline(std::cin, line)) {
    instructions.push_back(Instruction::instruction(line));
  }

  State state(instructions);
  state.reg(Register::a, 12);
  state.execute();
  std::cout << "Part 2 a = " << state.reg(Register::a) << '\n';
}
