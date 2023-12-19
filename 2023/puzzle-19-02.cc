#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Int = std::int64_t;
using UInt = std::uint64_t;

constexpr bool debug{true};

enum Op { greater, lessthan };

using Ratings = std::unordered_map<char, UInt>;
using Range = std::pair<UInt, UInt>;
using RatingRanges = std::unordered_map<char, Range>;
using WorkList = std::list<std::pair<std::string, RatingRanges>>;

auto operator<<(std::ostream& os, RatingRanges const& ranges) -> std::ostream&
{
  auto const* init = "{";
  for (auto const& r : ranges) {
    os << init << r.first << '[' << r.second.first << ", " << r.second.second << ')';
    init = ", ";
  }
  return os << "}";
}

struct Action
{
  Action(char c, Op op, UInt limit, std::string next)
    : c_(c), op_(op), limit_(limit), next_(std::move(next))
  {
  }

  void split(WorkList& work_list, RatingRanges& r) const
  {
    auto it{r.find(c_)};
    assert(it != r.end());
    if (op_ == Op::greater) {
      RatingRanges rnew{r};
      auto itnew{rnew.find(c_)};
      itnew->second.first = std::max(itnew->second.first, limit_ + 1);
      if (itnew->second.first < itnew->second.second) {
        if (debug) {
          std::cout << "    " << next_ << ", " << rnew << '\n';
        }
        work_list.emplace_back(next_, rnew);
      }

      it->second.second = std::min(itnew->second.second, limit_ + 1);
      return;
    }

    assert(op_ == Op::lessthan);
    RatingRanges rnew{r};
    auto itnew{rnew.find(c_)};
    itnew->second.second = std::min(itnew->second.second, limit_);
    if (itnew->second.first < itnew->second.second) {
      if (debug) {
        std::cout << "    " << next_ << ", " << rnew << '\n';
      }
      work_list.emplace_back(next_, rnew);
    }

    it->second.first = std::max(itnew->second.first, limit_);
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

  void append_ranges(WorkList& work_list, RatingRanges r) const
  {
    for (auto const& action : actions_) {
      action.split(work_list, r);
    }

    if (debug) {
      std::cout << "    " << default_ << ", " << r << '\n';
    }
    work_list.emplace_back(default_, r);
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
      }
      break;
      default: {
        auto const default_start = --pos;
        while (line[++pos] != '}') {
          /* loop */
        }
        workflow.set_default(line.substr(default_start, pos - default_start));
      }
      break;
      }
      ++pos;
    }

    workflows_.insert({name, workflow});
  }

  [[nodiscard]] auto accept(Ratings const& r) const -> bool
  {
    RatingRanges ranges;
    for (auto [c, v] : r) {
      ranges.insert({c, {v, v + 1}});
    }
    return total_accepted(ranges) == 1;
  }

  [[nodiscard]] auto total_accepted(RatingRanges const& initial) const -> UInt
  {
    WorkList work_list;
    work_list.emplace_back("in", initial);
    UInt grand_total{0};

    while (!work_list.empty()) {
      auto [rule, range] = work_list.front();
      work_list.pop_front();

      if (debug) {
        std::cout << "  " << rule << ": " << range;
      }

      bool work_to_do{true};
      for (auto const& r : range) {
        work_to_do &= (r.second.first < r.second.second);
      }
      if (!work_to_do) {
        if (debug) {
          std::cout << " = 0; # No work to do.\n";
        }
        continue;
      }

      if (rule == "R") {
        if (debug) {
          std::cout << " = 0; # Reject.\n";
        }
        continue;
      }
      if (rule == "A") {
        UInt total{1};
        for (auto const& r : range) {
          total *= r.second.second - r.second.first;
        }
        grand_total += total;
        if (debug) {
          std::cout << " = " << total << '\n';
        }
        continue;
      }

      auto it = workflows_.find(rule);
      assert(it != workflows_.end());
      if (debug) { std::cout << '\n'; }
      it->second.append_ranges(work_list, range);
    }

    if (debug) {
      std::cout << "  Grand total: " << grand_total << '\n';
    }
    return grand_total;
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

  UInt total{0};
  while (std::getline(std::cin, line)) {
    Ratings const r = read_ratings(line);
    if (debug) { std::cout << line << ":\n"; }
    if (workflows.accept(r)) {
      for (auto [c, value] : r) {
        total += value;
      }
    }
  }
  std::cout << "Part 1 total: " << total << '\n';

  RatingRanges range;
  range.insert({'x', {1, 4001}});
  range.insert({'m', {1, 4001}});
  range.insert({'a', {1, 4001}});
  range.insert({'s', {1, 4001}});
  UInt const grand_total{workflows.total_accepted(range)};

  std::cout << "Total: " << grand_total << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
