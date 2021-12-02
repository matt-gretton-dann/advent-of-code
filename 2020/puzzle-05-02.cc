#include <array>
#include <cassert>
#include <iostream>
#include <string>

/** \brief Return the position described by BSP in s.
 *  \tparam L2    log2 of the max position.
 *  \tparam LOWER character which means use lower half.
 *  \tparam UPPER character which means use upper half.
 *  \param  s     String to parse, must be L2 characters long
 *  \return       Position described by s.
 *
 * \subsection After the fact commentary:
 *
 * We don't need to keep a record of high at all so the original solution
 * was over enthusiastic.
 */
template<unsigned L2, char LOWER, char UPPER>
unsigned find_pos(std::string const& s)
{
  assert(s.length() == L2);
  unsigned low = 0;
  unsigned width = 1 << (L2 - 1);

  for (auto c : s) {
    assert(width != 0);
    if (c == UPPER) {
      low = low + width;
    }
    else {
      assert(c == LOWER);
    }

    width >>= 1;
  }

  assert(width == 0);
  return low;
}

/** \brief      A seat
 *  \tparam L2R log 2 number of rows
 *  \tparam L2C log 2 number of columns
 */
template<unsigned L2R, unsigned L2C>
struct Seat
{
  /** \brief   Construct the seat.
   *  \param s String representing seat location, must be L2R + L2C chars.
   *
   * First L2R characters must be 'F' and 'B' representing row, last L2C
   * characters must by 'L', and 'R' representing column.
   *
   * \brief  After the fact commentary:
   *
   * Should have used initialiser lists.
   */
  Seat(std::string const& s)
      : row_(find_pos<L2R, 'F', 'B'>(s.substr(0, L2R))),
        col_(find_pos<L2C, 'L', 'R'>(s.substr(L2R, L2C)))
  {
    assert(s.length() == L2R + L2C);
    std::cout << s << ": row " << row_ << ", column " << col_ << ", seat ID " << id() << ".\n";
  }

  /** Get seat ID.  */
  unsigned id() const noexcept { return row_ * (1 << L2C) + col_; }

  unsigned row_;  ///< Seat row
  unsigned col_;  ///< Seat column
};

/** \brief main
 *
 * \subsection After the fact commentary:
 *
 * Could have initialised the array better.
 */
int main(int argc, char** argv)
{
  constexpr unsigned l2r = 7;                    ///< Log 2 number of rows
  constexpr unsigned l2c = 3;                    ///< Log 2 number of columns
  constexpr unsigned max_id = 1 << (l2r + l2c);  ///< Max ID.
  std::array<bool, max_id> id_present = {};      ///< Is the ID present?

  // Read the lines in marking IDs that are present
  for (std::string line; std::getline(std::cin, line);) {
    Seat<l2r, l2c> seat(line);
    assert(seat.id() < max_id);
    id_present[seat.id()] = true;
  }

  // We have an empty seat if it's ID is not present, but the IDs at +/-1 are.
  for (unsigned id = 1; id < id_present.size() - 1; ++id) {
    if (!id_present[id] && id_present[id - 1] && id_present[id + 1]) {
      std::cout << "Empty seat at: " << id << "\n";
    }
  }

  return 0;
}