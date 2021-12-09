#include <cassert>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>

struct Ranges
{
  auto operator==(Ranges const& rhs) const noexcept { return ranges_ == rhs.ranges_; }

  void add(std::uint32_t low, std::uint32_t high)
  {
    static constexpr auto max{std::numeric_limits<std::uint32_t>::max()};
    auto it{ranges_.begin()};
    while (it != ranges_.end()) {
      if (low != 0 && it->second < low - 1) {
        ++it;
      }
      else if (high == max || high + 1 < it->first) {
        ranges_.insert(it, {low, high});
        return;
      }
      else {
        it->first = std::min(low, it->first);
        it->second = std::max(high, it->second);
        auto it2{it + 1};
        while (it2 != ranges_.end() && (it->second == std::numeric_limits<std::uint32_t>::max() ||
                                        it2->first <= it->second + 1)) {
          it->second = std::max(it->second, it2->second);
          ++it2;
        }
        ranges_.erase(it + 1, it2);
        return;
      }
    }

    ranges_.emplace_back(low, high);
  }

  [[nodiscard]] auto inverse() const -> Ranges
  {
    Ranges result;
    auto it{ranges_.begin()};
    assert(it->first <= it->second);
    if (it == ranges_.end()) {
      result.ranges_.emplace_back(0, std::numeric_limits<std::uint32_t>::max());
      return result;
    }
    if (it->first != 0) {
      result.ranges_.emplace_back(0, it->first - 1);
    }
    auto low{it->second + 1};
    for (++it; it != ranges_.end(); ++it) {
      assert(low < it->first);
      assert(it->first <= it->second);
      result.ranges_.emplace_back(low, it->first - 1);
      low = it->second + 1;
    }
    if (low != 0) {
      result.ranges_.emplace_back(low, std::numeric_limits<std::uint32_t>::max());
    }
    return result;
  }

  [[nodiscard]] auto lowest() const -> std::uint32_t
  {
    if (ranges_.empty()) {
      return std::numeric_limits<std::uint32_t>::max();
    }
    return ranges_.front().first;
  }

  [[nodiscard]] auto count() const -> std::uint32_t
  {
    return std::accumulate(ranges_.begin(), ranges_.end(), std::uint32_t{0},
                           [](auto a, auto const& r) { return a + (r.second - r.first + 1); });
  }

private:
  std::vector<std::pair<std::uint32_t, std::uint32_t>> ranges_;
};

auto main() -> int
{
  static std::regex match_re{"(\\d+)-(\\d+)"};
  std::string line;
  std::smatch m;
  Ranges ranges;
  while (std::getline(std::cin, line)) {
    if (!std::regex_search(line, m, match_re)) {
      std::cerr << "Unable to match: " << line << '\n';
      return 1;
    }
    ranges.add(std::stoul(m.str(1)), std::stoul(m.str(2)));
  }

  auto inverse{ranges.inverse()};
  std::cout << "First available IP address: " << inverse.lowest() << '\n';
  std::cout << "Number of available IP addresses: " << inverse.count() << '\n';
  return 0;
}
