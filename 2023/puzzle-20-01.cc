#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class Level { low, high };

using UInt = std::uint64_t;
using Signal = std::tuple<std::string, std::string, Level>;
using Signals = std::list<Signal>;
using StringVector = std::vector<std::string>;

constexpr bool debug = false;

struct Node
{
  virtual ~Node() = 0;

  virtual void add_input(std::string const& node) = 0;

  void add_output(std::string const& node)
  {
    dests_.push_back(node);
  }

  void receive(std::back_insert_iterator<Signals> it, Signal const& signal)
  {
    auto out{output_level(signal)};
    if (!out) { return; }

    for (auto const& dest : dests_) {
      *it++ = std::make_tuple(std::get<1>(signal), dest, *out);
    }
  }

private:
  virtual auto output_level(Signal const& signal) noexcept -> std::optional<Level> = 0;

  StringVector dests_;
};

Node::~Node() = default;

struct ButtonNode final : public Node
{
  ~ButtonNode() override = default;

  void add_input(const std::string& node) override { std::abort(); }

private:
  auto output_level(const Signal& signal) noexcept -> std::optional<Level> override
  {
    return Level::low;
  }
};

struct BroadcasterNode final : public Node
{
  ~BroadcasterNode() override = default;

  void add_input([[maybe_unused]] const std::string& node) override
  {
  }

private:
  auto output_level(const Signal& signal) noexcept -> std::optional<Level> override
  {
    return std::get<2>(signal);
  }
};

struct FlipFlopNode final : public Node
{
  ~FlipFlopNode() override = default;

  void add_input(const std::string& node) override
  {
  }

private:
  auto output_level(const Signal& signal) noexcept -> std::optional<Level> override
  {
    if (std::get<2>(signal) == Level::high) { return std::nullopt; }

    output_ = (output_ == Level::low) ? Level::high : Level::low;
    return output_;
  }

  Level output_{Level::low};
};

struct ConjunctionNode final : public Node
{
  ~ConjunctionNode() override = default;

  void add_input(const std::string& node) override
  {
    inputs_.insert({node, Level::low});
  }

private:
  auto output_level(const Signal& signal) noexcept -> std::optional<Level> override
  {
    auto it = inputs_.find(std::get<0>(signal));
    assert(it != inputs_.end());
    it->second = std::get<2>(signal);

    Level output = Level::low;
    for (auto const& input : inputs_) {
      if (input.second == Level::low) {
        output = Level::high;
        break;
      }
    }

    return output;
  }

  std::unordered_map<std::string, Level> inputs_;
};

struct NodeMap
{
  NodeMap(StringVector rows)
  {
    auto button = new ButtonNode();
    button->add_output("broadcaster");
    nodes_.insert({"button", button});

    for (auto const& row : rows) {
      auto pos = 0;
      Node* node = make_node(row[0]);
      if (row[pos] == '%' || row[pos] == '&') {
        ++pos;
      }

      auto const name_start = pos;
      auto const name_end = row.find(' ', pos);
      auto [it, success] = nodes_.insert({row.substr(name_start, name_end - name_start), node});
      assert(success);
    }

    for (auto const& row : rows) {
      auto pos = (row[0] == '%' || row[0] == '&') ? 1 : 0;

      auto const from_start = pos;
      auto const from_end = row.find(' ', pos);
      assert(row.substr(from_end, 4) == " -> ");
      pos = from_end + 2;
      auto from = row.substr(from_start, from_end - from_start);
      auto const from_it = nodes_.find(from);
      assert(from_it != nodes_.end());

      while (pos < row.size()) {
        pos += 2;
        auto const to_start = pos;
        auto to_end = row.find(',', pos);
        if (to_end == std::string::npos) { to_end = row.size(); }
        auto to = row.substr(to_start, to_end - to_start);
        pos = to_end;

        from_it->second->add_output(to);

        auto const to_it = nodes_.find(to);
        if (to_it != nodes_.end()) {
          to_it->second->add_input(from);
        }
      }
    }
  }

  [[nodiscard]] auto press_button() -> std::pair<UInt, UInt>
  {
    Signals work_list;
    work_list.emplace_back("button", "broadcaster", Level::low);

    UInt low_sent{0};
    UInt high_sent{0};
    auto inserter{std::back_inserter(work_list)};
    while (!work_list.empty()) {
      auto [from, to, level] = work_list.front();
      work_list.pop_front();

      if (level == Level::low) {
        ++low_sent;
      }
      else { ++high_sent; }

      auto const it = nodes_.find(to);

      if constexpr (debug) {
        std::cout << from << " -" << (level == Level::low ? "low" : "high") << ": " << to << '\n';
      }
      if (it != nodes_.end()) {
        it->second->receive(inserter, std::make_tuple(from, to, level));
      }
    }

    return std::make_pair(low_sent, high_sent);
  }

  ~NodeMap()
  {
    for (auto const& node : nodes_) { delete node.second; }
  }

private:
  [[nodiscard]] static auto make_node(char const c) -> Node*
  {
    switch (c) {
    case '%':
      return new FlipFlopNode;
    case '&':
      return new ConjunctionNode;
    default:
      return new BroadcasterNode;
    }
  }

  std::unordered_map<std::string, Node*> nodes_;
};

auto main() -> int try {
  std::string line;
  StringVector lines;

  while (std::getline(std::cin, line)) {
    lines.push_back(line);
  }

  NodeMap nodes(lines);

  UInt total_low{0};
  UInt total_high{0};
  for (unsigned i = 0; i < 1'000; ++i) {
    auto [low, high] = nodes.press_button();
    total_low += low;
    total_high += high;
  }

  std::cout << "Total signals: " << total_low << " * " << total_high << " = " << total_low *
    total_high << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
