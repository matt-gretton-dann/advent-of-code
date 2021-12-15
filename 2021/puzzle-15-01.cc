#include <cassert>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using UInt = unsigned int;

struct Cost
{
  constexpr explicit Cost(UInt entry_cost) : entry_cost_(entry_cost) {}

  [[nodiscard]] constexpr auto entry_cost() const noexcept -> UInt { return entry_cost_; }
  [[nodiscard]] constexpr auto total_cost() const noexcept -> UInt { return total_cost_; }
  [[nodiscard]] constexpr auto visited() const noexcept -> bool { return visited_; }

  constexpr void total_cost(UInt total_cost) noexcept { total_cost_ = total_cost; }
  constexpr void update_total_cost(UInt v) noexcept
  {
    total_cost_ = std::min(entry_cost_ + v, total_cost_);
  }
  constexpr void visit() noexcept { visited_ = true; }

private:
  UInt entry_cost_;
  UInt total_cost_{std::numeric_limits<UInt>::max()};
  bool visited_{false};
};

using CostVector = std::vector<Cost>;

auto main() -> int
{
  std::string line;
  CostVector costs;
  UInt width{0};
  while (std::getline(std::cin, line) && !line.empty()) {
    if (width == 0) {
      width = line.size();
    }
    else {
      assert(width == line.size());
    }
    for (auto c : line) {
      costs.emplace_back(c - '0');
    }
  }

  costs[0].total_cost(0);

  while (true) {
    /* Note that the comparison below makes deliberate use of unsigned wrap-round.  */
    auto it{costs.begin()};
    UInt cost{std::numeric_limits<UInt>::max()};
    for (auto it2{costs.begin()}; it2 != costs.end(); ++it2) {
      if (!it2->visited() && it2->total_cost() < cost) {
        cost = it2->total_cost();
        it = it2;
      }
    }
    it->visit();
    auto current_cost{it->total_cost()};
    auto idx{std::distance(costs.begin(), it)};
    if (idx == costs.size() - 1) {
      std::cout << "Total cost: " << current_cost;
      return 0;
    }
    if (idx % width != 0) {
      costs.at(idx - 1).update_total_cost(current_cost);
    }
    if ((idx + 1) % width != 0) {
      costs.at(idx + 1).update_total_cost(current_cost);
    }
    if (idx > width) {
      costs.at(idx - width).update_total_cost(current_cost);
    }
    if (idx + width < costs.size()) {
      costs.at(idx + width).update_total_cost(current_cost);
    }
  }
}
