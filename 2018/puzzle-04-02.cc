//
// Created by Matthew Gretton-Dann on 01/12/2022.
//

#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <utility>
#include <vector>

using UInt = std::uint64_t;

using namespace std::string_literals;

struct Event
{
  explicit Event(std::string const& s)
  {
    UInt pos{1};
    std::size_t l{0};
    std::stoull(s.substr(pos));
    timestamp_ = std::stoull(s.substr(pos), &l); // year
    pos += l + 1;
    timestamp_ *= 12;
    timestamp_ += std::stoull(s.substr(pos), &l) - 1; // month
    pos += l + 1;
    timestamp_ *= 31;                                 // Just need order not actual values.
    timestamp_ += std::stoull(s.substr(pos), &l) - 1; // day
    pos += l + 1;
    timestamp_ *= 24;
    timestamp_ += std::stoull(s.substr(pos), &l); // hour
    pos += l + 1;
    timestamp_ *= 60;
    timestamp_ += std::stoull(s.substr(pos), &l); // minute
    pos += l + 2;
    if (s.substr(pos) == "falls asleep"s) {
      wake_up_ = false;
      guard_ = 0;
    }
    else if (s.substr(pos) == "wakes up"s) {
      wake_up_ = true;
      guard_ = 0;
    }
    else {
      wake_up_ = true;
      guard_ = std::stoull(s.substr(pos + 7));
      assert(guard_ != 0);
    }
  }

  [[nodiscard]] auto wakes_up() const noexcept -> bool { return wake_up_; }
  [[nodiscard]] auto guard_id() const noexcept -> UInt { return guard_; }
  [[nodiscard]] auto timestamp() const noexcept -> UInt { return timestamp_; }

  [[nodiscard]] auto operator<(Event const& r) const noexcept -> bool
  {
    if (timestamp_ < r.timestamp_) {
      return true;
    }
    if (timestamp_ > r.timestamp_) {
      return false;
    }
    if (r.wake_up_) {
      return false;
    }
    return wake_up_;
  }

private:
  UInt timestamp_{0};
  UInt guard_{0};
  bool wake_up_{false};
};

auto print_timestamp(UInt timestamp)
{
  std::cout << "[" << timestamp << ": " << timestamp / static_cast<UInt>(24 * 60 * 31 * 12) << "-"
    << std::setw(2) << std::setfill('0')
    << 1 + timestamp / static_cast<UInt>(24 * 60 * 31) % 12 << "-" << std::setw(2)
    << std::setfill('0') << 1 + (timestamp / static_cast<UInt>(24 * 60)) % 31 << " "
    << std::setw(2) << std::setfill('0') << (timestamp / 60) % 24 << ":" << std::setw(2)
    << std::setfill('0') << timestamp % 60 << "]";
}

auto how_sleepy(std::vector<UInt> const& guard) -> UInt
{
  return std::accumulate(guard.begin(), guard.end(), UInt{0}, [](UInt a, UInt b) { return a + b; });
}

auto main() -> int
{
  std::string line;
  std::vector<Event> events;

  while (std::getline(std::cin, line)) {
    events.emplace_back(line);
  }
  std::sort(events.begin(), events.end());
  std::map<UInt, std::vector<UInt>> guard_sleep_map;
  auto current_it{guard_sleep_map.begin()};
  UInt asleep_from{0};

  for (auto const& e : events) {
    if (e.wakes_up()) {
      if (e.guard_id() != 0) {
        auto result = guard_sleep_map.insert({e.guard_id(), std::vector<UInt>(60, 0)});
        current_it = result.first;
        assert(asleep_from == 0);
        print_timestamp(e.timestamp());
        std::cout << ": Guard " << current_it->first << " starts duty.\n";
      }
      else {
        assert(asleep_from != 0);
        for (UInt i = asleep_from; i != e.timestamp(); ++i) {
          ++(current_it->second[i % 60]);
        }
        print_timestamp(e.timestamp());
        std::cout << ": Guard " << current_it->first << " wakes up after "
          << e.timestamp() - asleep_from << " minutes sleep.  Has slept a total of "
          << how_sleepy(current_it->second) << " minutes.\n";
        asleep_from = 0;
      }
    }
    else {
      assert(asleep_from == 0);
      asleep_from = e.timestamp();
      print_timestamp(e.timestamp());
      std::cout << ": Guard " << current_it->first << " goes to sleep.\n";
    }
  }

  UInt max_minute{0};
  UInt max_score{0};
  for (auto const& guard_sleep : guard_sleep_map) {
    auto sleepiest_minute{std::max_element(guard_sleep.second.begin(), guard_sleep.second.end())};
    UInt const score{guard_sleep.first * (sleepiest_minute - guard_sleep.second.begin())};
    if (*sleepiest_minute > max_minute) {
      max_score = score;
      max_minute = *sleepiest_minute;
    }

    std::cout << "Guard " << guard_sleep.first << " slept most for " << *sleepiest_minute
      << " minutes at " << sleepiest_minute - guard_sleep.second.begin()
      << " score: " << score << "\n";
  }

  std::cout << "Result: " << max_score << "\n";
  return EXIT_SUCCESS;
}
