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

/* A location: Really just an ordered pair of x, y co-ordinates.
 * We supply an ordering which means that when we iterate over the map we start in the top left
 * and go along a row at a time.
 */
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

/* The data we store at each location - effectively whether a horizontal line to the right
 * and/or a vertical line down starts or ends here.
 */
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
  /* Add an instruction.
   *
   * We store instructions in a map of each corner giving the location and whether a line
   * extends right and/or down from this corner.
   */
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

  /* Count the number of interior cells.
   *
   * The fundamental algorithm used is to scan each row and everytime we encounter a wall that
   * has a connection down we toggle whether we count ourselves as inside or not.
   * We then count all cells that are inside and/or a wall.
   *
   * This is complicated by the fact that the grid is so large (O(10^14) is the result).
   *
   * So we have stored only the corners.  We iterate over each line and generate the vertical edges
   * as we see them storing them in vertical_set.
   */
  [[nodiscard]] auto count_interior() const noexcept -> UInt
  {
    assert(current_loc_.x() == 0);
    assert(current_loc_.y() == 0);

    std::set<Int> vertical_set;
    Int y{corners_.begin()->first.y()}; // Start on the first row
    Int x{min_x_};                      // At the far left
    bool inside{false};
    bool horiz_on{false};
    UInt count{0};
    constexpr bool debug{true}; // Display debug info as we go along?

    for (auto const& [loc, data] : corners_) {
      /* Handle each corner.  Because of the way we have ordered the map this presents them
       * row by row, left to right, top-to-bottom. */

      if (y != loc.y()) {
        /* New row.  Complete any left over entries from the previous row. */
        assert(!horiz_on); // Should not be running a horizontal edge.

        /* Find the remaining vertical edges on the row and handle them.  We use x - 1 in the
         * bounds check as we haven't checked to see if x contains a vertical edge yet.
         */
        for (auto it{vertical_set.upper_bound(x - 1)}; it != vertical_set.end(); ++it) {
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

        assert(!inside); // End of a row should be outside.

        /* Now do the next row.  But there may be a set of rows with no entries - so we have to
         * just handle the repeating rows specially first of all.
         */
        ++y;
        auto const repeat_count{loc.y() - y};
        UInt row_count{0};
        inside = false;
        Int row_x{*vertical_set.begin()};
        if (debug) {
          std::cout << "R0: Repeating rows [" << y << ", " << loc.y() << ")\n";
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

        assert(!inside); // Shouldn't be inside at the end of a row.

        y = loc.y(); // Now set y to the next interesting row
        x = min_x_;  // x becomes the minimum value.
        inside = false;
        horiz_on = false;
      }

      /* Find the remaining vertical edges on the row and handle them.  We use x - 1 in the
       * bounds check as we haven't checked to see if x contains a vertical edge yet.
       */
      for (auto it{vertical_set.upper_bound(x - 1)}; it != vertical_set.end() && *it < loc.x(); ++
           it) {
        // We shouldn't be in a horizontal row and have vertical edges.
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
        /* We're inside or doing a horizontal wall count this run.  Note we have to be careful
         * as the complete cross product of inside and horiz_on states is possible, and we don't
         * want to double count.
         */
        count += loc.x() - x;
        if (debug) {
          std::cout << count << ": " << (horiz_on ? "horizontal wall" : "inside new") << " [" << x
            << ", " << loc.x() << ")\n";
        }
      }
      ++count;
      if (debug) {
        std::cout << count << ": wall new [" << loc.x() << "] = " << loc << " - " << data << "\n";
      }
      x = loc.x() + 1;

      /* Update the set of vertical edges. */
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

    assert(!inside);
    assert(!horiz_on);

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
