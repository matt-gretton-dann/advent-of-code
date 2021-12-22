#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <variant>
#include <vector>

using Int = long;
using Register = char;
using Operand = std::variant<Register, Int, std::monostate>;
using Messages = std::vector<Int>;
enum class Opcode { snd, set, add, mul, mod, rcv, jgz };

auto to_opcode(std::string const& s) -> Opcode
{
  if (s == "snd") {
    return Opcode::snd;
  }
  if (s == "set") {
    return Opcode::set;
  }
  if (s == "add") {
    return Opcode::add;
  }
  if (s == "mul") {
    return Opcode::mul;
  }
  if (s == "mod") {
    return Opcode::mod;
  }
  if (s == "rcv") {
    return Opcode::rcv;
  }
  if (s == "jgz") {
    return Opcode::jgz;
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

  auto out_queue_begin() const noexcept -> Messages::const_iterator { return out_queue_.begin(); }
  auto out_queue_end() const noexcept -> Messages::const_iterator { return out_queue_.end(); }
  auto out_queue_size() const noexcept -> Messages::size_type { return out_queue_.size(); }

  void set(Register reg, Int value) { registers_.insert_or_assign(reg, value); }

  template<typename It>
  auto execute(It in_queue_begin, It in_queue_end) -> It
  {
    while (pc_ >= 0 && pc_ < instructions_.size()) {
      auto const& instruction{instructions_[pc_]};
      switch (instruction.opcode()) {
      case Opcode::add:
        set(instruction.op1(), value(instruction.op1()) + value(instruction.op2()));
        break;
      case Opcode::jgz:
        if (value(instruction.op1()) > 0) {
          pc_ += value(instruction.op2()) - 1;
        }
        break;
      case Opcode::mod:
        set(instruction.op1(), value(instruction.op1()) % value(instruction.op2()));
        break;
      case Opcode::mul:
        set(instruction.op1(), value(instruction.op1()) * value(instruction.op2()));
        break;
      case Opcode::set:
        set(instruction.op1(), value(instruction.op2()));
        break;
      case Opcode::snd:
        out_queue_.push_back(value(instruction.op1()));
        break;
      case Opcode::rcv:
        if (in_queue_begin == in_queue_end) {
          return in_queue_begin;
        }
        set(instruction.op1(), *(in_queue_begin++));
        break;
      default:
        abort();
      }
      ++pc_;
    }

    return in_queue_begin;
  }

private:
  void set(Operand const& op, Int value)
  {
    if (std::holds_alternative<Register>(op)) {
      set(std::get<Register>(op), value);
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
  Messages out_queue_;
  Int pc_{0};
};

auto main() -> int
{
  Instructions instructions;
  std::string line;
  while (std::getline(std::cin, line) && !line.empty()) {
    instructions.push_back(Instruction::instruction(line));
  }

  State cpu0{instructions};
  cpu0.set('p', 0);
  State cpu1{instructions};
  cpu1.set('p', 1);

  bool cont = true;
  std::size_t queue_used0{0};
  std::size_t queue_used1{0};
  while (cont) {
    /* Iterate over each CPU in turn, monitoring how queue sizes change.  */
    auto queue1{cpu0.execute(cpu1.out_queue_begin() + queue_used1, cpu1.out_queue_end())};
    auto next_queue_used1{std::distance(cpu1.out_queue_begin(), queue1)};
    auto queue0{cpu1.execute(cpu0.out_queue_begin() + queue_used0, cpu0.out_queue_end())};
    auto next_queue_used0{std::distance(cpu0.out_queue_begin(), queue0)};
    if (next_queue_used0 == queue_used0 && next_queue_used1 == queue_used1) {
      /* We have deadlocked/finished if neither queue has changed in size this iteration.  */
      std::cout << "CPU 0 out queue size: " << cpu0.out_queue_size() << '\n';
      std::cout << "CPU 1 out queue size: " << cpu1.out_queue_size() << '\n';
      return 0;
    }
    queue_used0 = next_queue_used0;
    queue_used1 = next_queue_used1;
  }
  return 0;
}
