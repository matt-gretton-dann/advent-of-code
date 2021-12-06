//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <array>
#include <iostream>
#include <regex>
#include <string>
#include <variant>

enum class Register { a, b, c, d };
enum class Opcode { copy_value, copy_reg, inc, dec, jnz, jmp };
using Int = std::int64_t;

auto to_register(std::string const& s) -> Register
{
  if (s == "a") {
    return Register::a;
  }
  if (s == "b") {
    return Register::b;
  }
  if (s == "c") {
    return Register::c;
  }
  if (s == "d") {
    return Register::d;
  }
  abort();
}

struct Instruction
{
  Opcode op_;
  Register dest_;
  std::variant<Register, Int, std::monostate> source_;

  static auto decode(std::string const& s) -> Instruction
  {
    static std::regex cpy_value{"cpy (-?\\d+) ([a-d])"};
    static std::regex cpy_reg{"cpy ([a-d]) ([a-d])"};
    static std::regex inc_re{"inc ([a-d])"};
    static std::regex dec_re{"dec ([a-d])"};
    static std::regex jnz_reg_re{"jnz ([a-d]) (-?\\d+)"};
    static std::regex jnz_value_re{"jnz (-?\\d+) (-?\\d+)"};

    std::smatch m;
    if (std::regex_search(s, m, cpy_value)) {
      return copy(std::stol(m.str(1)), to_register(m.str(2)));
    }
    if (std::regex_search(s, m, cpy_reg)) {
      return copy(to_register(m.str(1)), to_register(m.str(2)));
    }
    if (std::regex_search(s, m, inc_re)) {
      return inc(to_register(m.str(1)));
    }
    if (std::regex_search(s, m, dec_re)) {
      return dec(to_register(m.str(1)));
    }
    if (std::regex_search(s, m, jnz_reg_re)) {
      return jnz(to_register(m.str(1)), std::stol(m.str(2)));
    }
    if (std::regex_search(s, m, jnz_value_re)) {
      return jnz(std::stol(m.str(1)), std::stol(m.str(2)));
    }
    abort();
  }

  static auto copy(Register source, Register dest) -> Instruction
  {
    return {Opcode::copy_reg, dest, source};
  }
  static auto copy(Int source, Register dest) -> Instruction
  {
    return {Opcode::copy_value, dest, source};
  }
  static auto inc(Register dest) -> Instruction { return {Opcode::inc, dest, std::monostate{}}; }
  static auto dec(Register dest) -> Instruction { return {Opcode::dec, dest, std::monostate{}}; }
  static auto jnz(Register cmp, Int offset) -> Instruction { return {Opcode::jnz, cmp, offset}; }
  static auto jnz(Int cmp, Int offset) -> Instruction
  {
    return {Opcode::jmp, Register::a, cmp == 0 ? 1 : offset};
  }
};

struct State
{
  [[nodiscard]] auto value(Register r) const noexcept -> Int
  {
    return values_[static_cast<unsigned>(r)];
  }

  [[nodiscard]] auto pc() const noexcept -> unsigned { return pc_; }

  void execute(Instruction const& instruction)
  {
    Int& dest_reg{values_[static_cast<unsigned>(instruction.dest_)]};
    switch (instruction.op_) {
    case Opcode::copy_value:
      dest_reg = std::get<Int>(instruction.source_);
      break;
    case Opcode::copy_reg:
      dest_reg = values_[static_cast<unsigned>(std::get<Register>(instruction.source_))];
      break;
    case Opcode::inc:
      ++dest_reg;
      break;
    case Opcode::dec:
      --dest_reg;
      break;
    case Opcode::jnz:
      if (dest_reg != 0) {
        pc_ += std::get<Int>(instruction.source_) - 1;
      }
      break;
    case Opcode::jmp:
      pc_ += std::get<Int>(instruction.source_) - 1;
      break;
    }

    ++pc_;
  }

private:
  std::array<Int, 4> values_{0, 0, 0, 0};
  unsigned pc_{0};
};

auto main() -> int
{
  std::string line;
  std::vector<Instruction> instructions;
  while (std::getline(std::cin, line)) {
    instructions.push_back(Instruction::decode(line));
  }

  State state;
  while (state.pc() < instructions.size()) {
    state.execute(instructions[state.pc()]);
  }

  std::cout << "a = " << state.value(Register::a) << '\n';
  return 0;
}