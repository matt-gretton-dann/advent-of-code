#include <algorithm>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

enum class Opcode { Acc, Jmp, Nop };
using Value = int;

struct Instruction {
  Instruction(std::string const &s)
      : op_(to_opcode(s.substr(0, 3))), v_(to_value(s.substr(4))) {}

  Opcode opcode() const noexcept { return op_; }
  Value value() const noexcept { return v_; }

private:
  Opcode to_opcode(std::string const &s) {
    if (s == "acc") {
      return Opcode::Acc;
    } else if (s == "jmp") {
      return Opcode::Jmp;
    } else if (s == "nop") {
      return Opcode::Nop;
    } else {
      assert(false);
      return Opcode::Nop;
    }
  }

  Value to_value(std::string const &s) {
    int sign = 0;
    int v = 0;
    std::size_t pos = 0;
    if (s[0] == '+') {
      sign = 1;
    } else if (s[0] == '-') {
      sign = -1;
    } else {
      assert(false);
    }
    v = std::stoi(s.substr(1), &pos);
    assert(pos == s.length() - 1);
    return v * sign;
  }

  Opcode op_;
  Value v_;
};

std::ostream &operator<<(std::ostream &os, Instruction const &i) {
  switch (i.opcode()) {
  case Opcode::Acc:
    os << "acc";
    break;
  case Opcode::Jmp:
    os << "jmp";
    break;
  case Opcode::Nop:
    os << "nop";
    break;
  default:
    os << "UNK";
    break;
  }
  os << " ";
  if (i.value() >= 0) {
    os << "+";
  }
  os << i.value();
  return os;
}
using Instructions = std::vector<Instruction>;

struct VM {
  VM() : pc_(0), acc_(0) {}

  void add_instruction(Instruction const &i) {
    std::cout << i << "\n";
    instrs_.push_back(i);
  }

  void execute() {
    std::vector<bool> seen(instrs_.size(), false);
    while (!seen[pc_]) {
      assert(pc_ < instrs_.size());
      seen[pc_] = true;
      execute(instrs_[pc_]);
      ++pc_;
    }
    std::cout << "PC seen before: " << pc_ << "\n";
  }

  Value acc() const noexcept { return acc_; }

private:
  void execute(Instruction const &i) {
    std::cout << pc_ << ": " << i;
    switch (i.opcode()) {
    case Opcode::Acc:
      std::cout << "; acc = " << acc_ << " + " << i.value();
      acc_ += i.value();
      std::cout << " = " << acc_;
      break;
    case Opcode::Jmp:
      std::cout << "; pc = " << pc_ << " + " << i.value();
      pc_ += i.value() - 1;
      std::cout << " = " << pc_ + 1;
      break;
    case Opcode::Nop:
      break;
    }
    std::cout << "\n";
  }

  Instructions instrs_;
  std::size_t pc_;
  Value acc_;
};

int main(int argc, char **argv) {
  VM vm;
  for (std::string line; std::getline(std::cin, line);) {
    vm.add_instruction(Instruction(line));
  }

  vm.execute();
  std::cout << "Accumulator: " << vm.acc() << "\n";

  return 0;
}