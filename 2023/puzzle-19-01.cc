#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

enum Op { greater, lessthan };

using Ratings = std::unordered_map<char, UInt>;

struct Action
{
  Action(char c, Op op, UInt limit, std::string next)
    : c_(c), op_(op), limit_(limit), next_(std::move(next))
  {
  }

  [[nodiscard]] auto holds(Ratings const& r) const noexcept -> bool
  {
    auto const it{r.find(c_)};
    assert(it != r.end());
    if (op_ == Op::greater) { return it->second > limit_; }
    return it->second < limit_;
  }

  [[nodiscard]] auto next() const noexcept -> std::string const& { return next_; }

private:
  char c_;
  Op op_;
  UInt limit_;
  std::string next_;
};

struct Workflow
{
  void set_default(std::string const& d) { default_ = d; }
  void push_action(Action const& action) { actions_.push_back(action); }

  [[nodiscard]] auto run(Ratings const& r) const noexcept -> std::string const&
  {
    for (auto const& action : actions_) {
      if (action.holds(r)) { return action.next(); }
    }
    return default_;
  }

private:
  std::string default_;
  std::vector<Action> actions_;
};

struct Workflows
{
  void insert_workflow(std::string const& line)
  {
    auto pos{line.find('{')};
    auto name = line.substr(0, pos);
    ++pos;

    Workflow workflow;
    std::cout << "Workflow '" << name << "'\n";
    while (pos < line.size()) {
      char const c = line[pos++];
      switch (line[pos]) {
      case '>':
      case '<': {
        Op const op = line[pos++] == '>' ? Op::greater : Op::lessthan;
        char* ppos = nullptr;
        UInt const limit = std::strtoull(line.data() + pos, &ppos, 10);
        pos = ppos - line.data();
        assert(line[pos] == ':');
        auto const next_start = ++pos;
        while (line[++pos] != ',') {
          /* loop */
        }
        Action const action(c, op, limit, line.substr(next_start, pos - next_start));
        workflow.push_action(action);
        std::cout << "  " << c << ' ' << (op == Op::greater ? '>' : '<') << ' ' << limit << ' ' <<
          line.substr(next_start, pos - next_start) << '\n';
      }
      break;
      default: {
        auto const default_start = --pos;
        while (line[++pos] != '}') {
          /* loop */
        }
        workflow.set_default(line.substr(default_start, pos - default_start));
        std::cout << "  default " << line.substr(default_start, pos - default_start) << '\n';
      }
      break;
      }
      ++pos;
    }

    workflows_.insert({name, workflow});
  }

  [[nodiscard]] auto accept(Ratings const& r) const -> bool
  {
    std::string rule = "in";
    while (true) {
      std::cout << rule;
      if (rule == "A") { return true; }
      if (rule == "R") { return false; }
      std::cout << " -> ";
      auto it = workflows_.find(rule);
      assert(it != workflows_.end());
      rule = it->second.run(r);
    }
  }

private:
  std::unordered_map<std::string, Workflow> workflows_;
};

auto read_ratings(std::string const& line) -> Ratings
{
  Ratings r;
  std::size_t pos = 1;
  while (pos < line.size()) {
    char const c = line[pos++];
    assert(line[pos] == '=');
    char* ppos = nullptr;
    UInt const value = std::strtoull(line.data() + pos + 1, &ppos, 10);
    r.insert({c, value});
    pos = ppos - line.data() + 1;
  }

  return r;
}

auto main() -> int try {
  std::string line;
  Workflows workflows;

  while (std::getline(std::cin, line) && !line.empty()) {
    workflows.insert_workflow(line);
  }

  Ratings total;
  while (std::getline(std::cin, line)) {
    Ratings const r = read_ratings(line);
    std::cout << line << ": ";
    if (workflows.accept(r)) {
      for (auto [c, value] : r) {
        auto [it, success] = total.insert({c, 0});
        it->second += value;
      }
    }
    std::cout << '\n';
  }

  UInt const grand_total = std::accumulate(total.begin(), total.end(), UInt{0},
                                           [](UInt a, auto b) { return a + b.second; });

  std::cout << "Total: " << grand_total << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
