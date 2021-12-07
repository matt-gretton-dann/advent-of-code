#include <iostream>
#include <regex>
#include <string>

struct Ranges
{
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
          it->second = it2->second;
          ++it2;
        }
        ranges_.erase(it + 1, it2);
        return;
      }
    }

    ranges_.push_back({low, high});
  }

  Ranges inverse() const
  {
    Ranges result;
    auto it{ranges_.begin()};
    if (it == ranges_.end()) {
      result.ranges_.push_back({0, std::numeric_limits<std::uint32_t>::max()});
      return result;
    }
    if (it->first != 0) {
      result.ranges_.push_back({0, it->first - 1});
    }
    auto low{it->second + 1};
    while (it != ranges_.end()) {
      result.ranges_.push_back({low, it->first - 1});
      low = it->second + 1;
      ++it;
    }
    if (low != 0) {
      result.ranges_.push_back({low, std::numeric_limits<std::uint32_t>::max()});
    }
    return result;
  }

  auto lowest() const -> std::uint32_t
  {
    if (ranges_.empty()) {
      return std::numeric_limits<std::uint32_t>::max();
    }
    return ranges_.front().first;
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

  std::cout << "First available IP address: " << ranges.inverse().lowest() << '\n';
  return 0;
}