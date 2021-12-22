#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include <vector>

using UInt = unsigned long;
using Chain = std::pair<UInt, UInt>;

struct State
{
  void add(Chain const& chain) { chains_.push_back({chain, false}); }

  template<typename Inserter>
  void next_states(Inserter inserter) const
  {
    State next(*this);
    for (auto& chain : next.chains_) {
      if (chain.second) {
        continue;
      }
      if (chain.first.first == tail_end_) {
        next.tail_end_ = chain.first.second;
        chain.second = true;
        *inserter++ = next;
      }
      if (chain.first.second == tail_end_) {
        next.tail_end_ = chain.first.first;
        chain.second = true;
        *inserter++ = next;
      }
      chain.second = false;
    }
  }

  [[nodiscard]] auto strength() const noexcept -> UInt
  {
    return std::accumulate(chains_.begin(), chains_.end(), UInt{0}, [](UInt a, auto const& c) {
      if (c.second) {
        return a + c.first.first + c.first.second;
      }
      else {
        return a;
      }
    });
  }

private:
  UInt tail_end_{0};
  std::vector<std::pair<Chain, bool>> chains_;
};

auto main() -> int
{
  std::regex re{"(\\d+)/(\\d+)"};
  std::string line;
  State initial_state;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, re)) {
      std::cerr << "Unable to interpret: " << line << '\n';
      return 1;
    }
    initial_state.add({std::stoul(m.str(1)), std::stoul(m.str(2))});
  }

  auto strength{initial_state.strength()};
  std::vector<State> states;
  states.push_back(initial_state);
  while (!states.empty()) {
    std::vector<State> next_states;
    strength = 0;
    auto inserter{std::back_inserter(next_states)};
    for (auto const& state : states) {
      strength = std::max(strength, state.strength());
      state.next_states(inserter);
    }
    std::swap(states, next_states);
  }

  std::cout << "Max strength: " << strength << '\n';
  return 0;
}
