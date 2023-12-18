#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>

using Int = std::int64_t;
using UInt = std::uint64_t;

enum class State { on, off };

struct Location
{
  Location(Int const x, Int const y) noexcept
    : x_(x), y_(y)
  {
  }

  [[nodiscard]] auto operator==(Location const& rhs) const noexcept -> bool
  {
    return x_ == rhs.x_ && y_ == rhs.y_;
  }

  [[nodiscard]] auto operator<=>(const Location& rhs) const noexcept -> std::strong_ordering
  {
    if (y_ == rhs.y_) { return x_ <=> rhs.x_; }
    return y_ <=> rhs.y_;
  }

  [[nodiscard]] auto x() const noexcept -> Int { return x_; }
  [[nodiscard]] auto y() const noexcept -> Int { return y_; }

private:
  Int x_;
  Int y_;
};

struct Data
{
  void horiz(State const horiz) noexcept { horiz_ = horiz; }
  void vert(State const vert) noexcept { vert_ = vert; }
  [[nodiscard]] auto horiz() const noexcept -> State { return horiz_; }
  [[nodiscard]] auto vert() const noexcept -> State { return vert_; }

private:
  State horiz_{State::off};
  State vert_{State::off};
};

template<>
struct std::hash<Location>
{
  [[nodiscard]] auto operator()(Location const& loc) const noexcept -> std::size_t
  {
    return std::hash<Int>{}((loc.x() << 16) + loc.y());
  }
};

auto operator<<(std::ostream& os, Location const& loc) -> std::ostream&
{
  return os << '(' << loc.x() << ", " << loc.y() << ')';
}

auto operator<<(std::ostream& os, State state) -> std::ostream&
{
  if (state == State::on) { return os << "on"; }
  return os << "off";
}

auto operator<<(std::ostream& os, Data const& data) -> std::ostream&
{
  return os << '(' << data.horiz() << ", " << data.vert() << ')';
}

struct Grid
{
  void add_instruction(std::string const& line)
  {
    char dir;
    char* pos = nullptr;
    std::strtoll(line.data() + 2, &pos, 10);
    pos += 3;
    Int const colour = std::strtoll(pos, nullptr, 16);

    switch (colour & 0xf) {
    case 0:
      dir = 'R';
      break;
    case 1:
      dir = 'D';
      break;
    case 2:
      dir = 'L';
      break;
    case 3:
      dir = 'U';
      break;
    default:
      std::abort();
    }

    Int const amt = colour / 16;

    Int const dx = amt * (dir == 'R' ? 1 : (dir == 'L' ? -1 : 0));
    Int const dy = amt * (dir == 'D' ? 1 : (dir == 'U' ? -1 : 0));

    if (dir == 'L' || dir == 'R') {
      auto [it, success] = corners_.insert(std::make_pair(current_loc_, Data()));
      it->second.horiz(dir == 'R' ? State::on : State::off);
    }
    if (dir == 'U' || dir == 'D') {
      auto [it, success] = corners_.insert(std::make_pair(current_loc_, Data()));
      it->second.vert(dir == 'D' ? State::on : State::off);
    }

    current_loc_ = Location(current_loc_.x() + dx, current_loc_.y() + dy);
    min_x_ = std::min(current_loc_.x(), min_x_);

    if (dir == 'L' || dir == 'R') {
      auto [it, success] = corners_.insert(std::make_pair(current_loc_, Data()));
      it->second.horiz(dir == 'L' ? State::on : State::off);
    }
    if (dir == 'U' || dir == 'D') {
      auto [it, success] = corners_.insert(std::make_pair(current_loc_, Data()));
      it->second.vert(dir == 'U' ? State::on : State::off);
    }
  }

  [[nodiscard]] auto count_interior() const noexcept -> UInt
  {
    assert(current_loc_.x() == 0);
    assert(current_loc_.y() == 0);

    std::set<Int> vertical_set;
    Int y{corners_.begin()->first.y()};
    Int x{min_x_};
    bool inside{false};
    bool horiz_on{false};
    UInt count{0};
    constexpr bool debug{true};

    for (auto const& [loc, data] : corners_) {
      if (y != loc.y()) {
        for (auto it{vertical_set.upper_bound(x)}; it != vertical_set.end(); ++it) {
          assert(!horiz_on);
          if (inside) {
            count += *it - x;
            if (debug) {
              std::cout << count << ": tail inside [" << x << ", " << *it << ")\n";
            }
          }
          ++count;
          inside = !inside;
          x = *it + 1;
          if (debug) {
            std::cout << count << ": tail wall [" << *it << "]\n";
          }
        }

        // Reset for a new line.
        assert(!inside);
        assert(!horiz_on);

        auto const repeat_count{loc.y() - y - 1};
        UInt row_count{0};
        inside = false;
        Int row_x{*vertical_set.begin()};
        if (debug) {
          std::cout << "R0: Repeating rows [" << y + 1 << ", " << loc.y() << ")\n";
        }
        for (auto const vert_entry : vertical_set) {
          if (inside) {
            row_count += vert_entry - row_x;
            if (debug) {
              std::cout << "R" << row_count << " inside: [" << row_x << ", " << vert_entry << ")\n";
            }
          }
          ++row_count;
          if (debug) {
            std::cout << "R" << row_count << " wall: [" << vert_entry << "]\n";
          }
          row_x = vert_entry + 1;
          inside = !inside;
        }
        count += row_count * repeat_count;
        if (debug) {
          std::cout << count << ": repeated rows " << row_count << " * " << repeat_count << '\n';
        }
        assert(!inside);

        y = loc.y();
        x = min_x_;
        inside = false;
        horiz_on = false;
      }

      for (auto it{vertical_set.upper_bound(x - 1)};
           it != vertical_set.end() && *it < loc.x(); ++it) {
        assert(!horiz_on);
        if (inside) {
          count += *it - x;
          if (debug) {
            std::cout << count << ": inside [" << x << ", " << *it << ")\n";
          }
        }
        ++count;
        if (debug) {
          std::cout << count << ": vertical wall [" << *it << "]\n";
        }
        inside = !inside;
        x = *it + 1;
      }
      if (inside || horiz_on) {
        count += loc.x() - x;
        if (debug) {
          if (horiz_on) {
            std::cout << count << ": horizontal wall [" << x << ", " << loc.x() << ")\n";
          }
          else {
            std::cout << count << ": inside new [" << x << ", " << loc.x() << ")\n";
          }
        }
      }
      ++count;
      if (debug) {
        std::cout << count << ": wall new [" << loc.x() << "] = " << loc << " - " <<
          data << "\n";
      }
      x = loc.x() + 1;

      horiz_on = data.horiz() == State::on;
      if (data.vert() == State::on) {
        inside = !inside;
        auto const [_, success] = vertical_set.insert(loc.x());
        assert(success);
      }
      else {
        assert(data.vert() == State::off);
        auto const success = vertical_set.erase(loc.x());
        assert(success != 0);
      }
    }

    return count;
  }

private :
  Location current_loc_{0, 0};
  std::map<Location, Data> corners_;
  Int min_x_{0};
};

auto main() -> int try {
  std::string line;
  Grid grid;

  while (std::getline(std::cin, line)) {
    grid.add_instruction(line);
  }

  UInt const count{grid.count_interior()};
  std::cout << "Total: " << count << '\n';

  return EXIT_SUCCESS;
}
catch (...) {
  std::cerr << "Uncaught exception.\n";
  return EXIT_FAILURE;
}
