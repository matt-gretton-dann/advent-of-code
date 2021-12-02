#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <variant>

// Algorithm overview:
//
// Because I'm lazy we basically build up a list of 'instructions' and
// repeatedly walking through them executing the ones we can.  We assume that
// each pass will achieve more than the previous pass as more signal values will
// have been determined (and they don't change between passes).  Eventually the
// VM reaches a steady state and at that point we can determine what the value
// of the wire 'a'.
//
// This is fast enough and simple enough for our purposes.
//
// A "better" way would be to start from the instruction that sets 'a' and then
// execute the instructions that determine the signals coming into it, and so
// on.  This would only require a single pass through the instruction list so
// would probably be significantly quicker.  (But requires more thought in the
// implementation)

// helper type for the visitor #4
template<class... Ts>
struct Overloaded : Ts...
{
  using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

/// Instruction action
enum class Action {
  Set,     ///< Set value
  And,     ///< And two values
  Or,      ///< Or two values
  LShift,  ///< Left shift
  RShift,  ///< Right shift
  Not      ///< Bitwise not
};

/// Pretty-print action
auto operator<<(std::ostream& os, Action act) -> std::ostream&
{
  switch (act) {
  case Action::Set:
    os << "SET";
    break;
  case Action::And:
    os << "AND";
    break;
  case Action::Or:
    os << "OR";
    break;
  case Action::LShift:
    os << "LSHIFT";
    break;
  case Action::RShift:
    os << "RSHIFT";
    break;
  case Action::Not:
    os << "NOT";
    break;
  }
  return os;
}

using Value = std::uint16_t;               ///< Value
using Wire = std::string;                  ///< Wire name (string)
using Signal = std::variant<Value, Wire>;  ///< Either a wire or explicit value

/// Outputter for a signal
auto operator<<(std::ostream& os, Signal const& signal) -> std::ostream&
{
  return std::visit(
    [&os](auto&& arg) -> std::ostream& {
      os << arg;
      return os;
    },
    signal);
}

/** \brief  An instruction.  */
struct Instruction
{
  /** \brief  Construct an instruction.
   *
   * \subsection Grammar
   *
   *    wire   := [a-z]+
   *    value  := [0-9]+
   *    signal := value | wire
   *    set    := signal '->' wire
   *    not    := 'NOT' set
   *    op     := 'AND' | 'OR' | 'LSHIFT' | 'RSHIFT'
   *    bin-op  := signal op signal '->' wire
   *    instr  := bin-op | not | set
   */
  explicit Instruction(std::string const& s)
  {
    if (parse_bin_op(s)) {
      return;
    }
    if (parse_not(s)) {
      return;
    }
    if (parse_set(s)) {
      return;
    }
    std::cout << "Unrecognised string: " << s << "\n";
    assert(false);
  }

  /// Get action
  [[nodiscard]] auto action() const noexcept -> Action { return act_; }

  /// Get the destination wire
  [[nodiscard]] auto dest() const noexcept -> Wire const& { return dest_; }

  /// Get the first (or only) source
  [[nodiscard]] auto src1() const noexcept -> Signal const& { return src1_; }

  /// Get the second source
  [[nodiscard]] auto src2() const noexcept -> Signal const&
  {
    assert(act_ != Action::Set && act_ != Action::Not);
    return src2_;
  }

private:
  /// Parse a <not> instruction.  Return true if successful.
  auto parse_not(std::string const& s) -> bool
  {
    if (s.substr(0, 4) == "NOT ") {
      std::string::size_type pos = 4;
      while (s[pos] == ' ') {
        ++pos;
      }
      return parse_set(s.substr(pos), Action::Not);
    }
    return false;
  }

  /// Parse a <bin_op> instruction.  Return true if successful.
  auto parse_bin_op(std::string const& s) -> bool
  {
    static const std::regex re("^([[:lower:][:digit:]]+) ([[:upper:]]+) "
                               "([[:lower:][:digit:]]+) -> ([[:lower:]]+)");
    std::smatch m;
    if (!std::regex_search(s, m, re)) {
      return false;
    }

    if (m.str(2) == "AND") {
      act_ = Action::And;
    }
    else if (m.str(2) == "OR") {
      act_ = Action::Or;
    }
    else if (m.str(2) == "LSHIFT") {
      act_ = Action::LShift;
    }
    else if (m.str(2) == "RSHIFT") {
      act_ = Action::RShift;
    }
    else {
      return false;
    }
    dest_ = m.str(4);
    src1_ = make_signal(m.str(1));
    src2_ = make_signal(m.str(3));
    std::cout << act_ << " " << dest_ << ", " << src1_ << ", " << src2_ << "\n";
    return true;
  }

  /// Parse a <set> instruction.
  ///
  /// Also used for the latter half of <not> parsing.  ACT tells you what is
  /// being parsed. Returns true if parsing successful.
  auto parse_set(std::string const& s, Action act = Action::Set) -> bool
  {
    static const std::regex re("^([[:lower:][:digit:]]+) -> ([[:lower:]]+)");
    std::smatch m;
    if (!std::regex_search(s, m, re)) {
      return false;
    }
    act_ = act;
    dest_ = m.str(2);
    src1_ = make_signal(m.str(1));
    std::cout << act_ << " " << dest_ << ", " << src1_ << "\n";
    return true;
  }

  /// Make a Signal from a string.
  static auto make_signal(std::string const& s) -> Signal
  {
    if (std::isdigit(s[0]) == 1) {
      auto u = std::stoul(s, nullptr, 10);
      assert(u <= UINT16_MAX);
      return {static_cast<std::uint16_t>(u)};
    }
    return {s};
  }

  Action act_{Action::Not};  ///< Action
  Wire dest_;                ///< Destination wire
  Signal src1_, src2_;       ///< Source signals
};

/// Outputter for an instruction.
auto operator<<(std::ostream& os, Instruction const& instr) -> std::ostream&
{
  os << instr.action() << " " << instr.dest() << ", " << instr.src1();
  if (instr.action() != Action::Set && instr.action() != Action::Not) {
    os << ", " << instr.src2();
  }
  return os;
}

/// Ma
using ValueMap = std::map<Wire, Value>;         ///< Map wires to values
using Instructions = std::vector<Instruction>;  ///< Instructions to execute

struct VM
{
  /// Add an instruction the the list we have
  void add_instr(Instruction const& instr) { instructions_.push_back(instr); }

  /// Has this wire a known value?
  [[nodiscard]] auto has_value(Wire const& w) const noexcept -> bool
  {
    return values_.find(w) != values_.end();
  }

  /// Has this signal a known value?
  [[nodiscard]] auto has_value(Signal const& s) const noexcept -> bool
  {
    return std::visit(Overloaded{[]([[maybe_unused]] Value v) { return true; },
                                 [&](Wire const& w) { return has_value(w); }},
                      s);
  }

  /// Get the value on the wire
  [[nodiscard]] auto value(Wire const& w) const noexcept -> Value
  {
    assert(has_value(w));
    return values_.find(w)->second;
  }

  /// Get the value of a signal
  [[nodiscard]] auto value(Signal const& s) const noexcept -> Value
  {
    return std::visit(
      Overloaded{[](Value v) { return v; }, [&](Wire const& w) { return value(w); }}, s);
  }

  /// Set the value of a wire
  void value(Wire const& w, Value value)
  {
    auto [it, success] = values_.insert({w, value});
    assert(success);
  }

  /// Set the value of a signal
  void value(Signal const& s, Value v)
  {
    std::visit(
      Overloaded{[v](Value v2) { assert(v == v2); },       // NOLINT(bugprone-lambda-function-name)
                 [&, v](Wire const& w) { value(w, v); }},  // NOLINT(bugprone-lambda-function-name)
      s);                                                  // NOLINT(bugprone-lambda-function-name)
  }

  /// Execute the instructions.  Returns true if we have updated some wire
  /// values.
  auto execute() -> bool
  {
    bool done_anything = false;
    for (auto const& instr : instructions_) {
      done_anything |= execute_instr(instr);
    }

    return done_anything;
  }

private:
  /** \brief        Attempt to execute an instruction
   *  \param  instr Instruction
   *  \return       True if instruction was executed.
   *
   * An instruction may not be executed if the incoming signals have not been
   * set yet.
   */
  auto execute_instr(Instruction const& instr) -> bool
  {
    std::cout << instr << " # ";

    // First check there is something to do - i.e. that the destination
    // register has not been set already.
    auto const& dest{instr.dest()};
    if (has_value(dest)) {
      std::cout << "already has value: " << dest << " = " << value(dest) << "\n";
      return false;
    }

    switch (instr.action()) {
    case Action::Set:
      return execute_single_src(instr, [](Value src) { return src; });
    case Action::Not:
      return execute_single_src(instr, [](Value src) { return ~src; });
    case Action::And:
      return execute_double_src(instr, [](Value src1, Value src2) { return src1 & src2; });
    case Action::Or:
      return execute_double_src(instr, [](Value src1, Value src2) { return src1 | src2; });
    case Action::LShift:
      return execute_double_src(instr, [](Value src1, Value src2) { return src1 << src2; });
    case Action::RShift:
      return execute_double_src(instr, [](Value src1, Value src2) { return src1 >> src2; });
    }

    return false;
  }

  /** \brief        Attempt to execute a single source instruction.
   *  \param  instr Instruction
   *  \param  fn    How to modify the source value to the dest.
   *  \return       True if we executed the function.
   */
  auto execute_single_src(Instruction const& instr, const std::function<Value(Value)>& fn) -> bool
  {
    auto const& dest{instr.dest()};
    Signal const& src{instr.src1()};
    if (has_value(src)) {
      value(dest, fn(value(src)));
      std::cout << "setting wire to: " << dest << " = " << value(dest) << "\n";
      return true;
    }

    std::cout << "missing value for signal: " << src << "\n";
    return false;
  }

  /** \brief        Attempt to execute a two source instruction.
   *  \param  instr Instruction
   *  \param  fn    How to modify the source values to the dest.
   *  \return       True if we executed the function.
   */
  auto execute_double_src(Instruction const& instr, const std::function<Value(Value, Value)>& fn)
    -> bool
  {
    auto const& dest{instr.dest()};
    auto const& src1{instr.src1()};
    auto const& src2{instr.src2()};
    if (has_value(src1) && has_value(src2)) {
      value(dest, fn(value(src1), value(src2)));
      std::cout << "setting wire to: " << dest << " = " << value(dest) << "\n";
      return true;
    }

    std::cout << "missing value for signals: " << src1 << ", " << src2 << "\n";
    return false;
  }

  ValueMap values_;
  Instructions instructions_;
};

auto main() -> int
{
  VM vm;

  // Parse the input
  for (std::string line; std::getline(std::cin, line);) {
    Instruction instr(line);
    vm.add_instr(instr);
  }

  // Execute the VM until it reaches a steady state
  bool changed = true;
  while (changed) {
    changed = vm.execute();
  }

  // Get the value of wire 'a'
  Wire a = Wire("a");
  std::cout << "a = ";
  if (!vm.has_value(a)) {
    std::cout << "UNSET\n";
  }
  else {
    std::cout << vm.value(a) << "\n";
  }

  return 0;
}