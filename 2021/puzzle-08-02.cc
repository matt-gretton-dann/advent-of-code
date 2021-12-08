//
// Created by Matthew Gretton-Dann on 08/12/2021.
//

#include <array>
#include <iostream>
#include <string>
#include <vector>

using Segments = std::string;
using Segment = char;

/**
 * \brief             If only one segment is lit return that segment, otherwise return '\0'
 * \param  segments   Segments lit
 * \return            Lit segment if only one or '\0' othherwise.
 */
auto unique_segment(Segments const& segments) -> Segment
{
  if (segments.size() == 1) {
    return segments[0];
  }
  return '\0';
}

/**
 * \brief           Return unique segment lit when \a remove & \a rest are removed from \a segments
 * \tparam Args     Types of \a rest
 * \param  segments Segments initially lit
 * \param  remove   Segment to remove from \a segments
 * \param  rest     Any more segments to remove
 * \return          Unique lit segment or '\0' if there isn't one
 */
template<typename... Args>
auto unique_segment(Segments const& segments, Segment remove, Args... rest) -> Segment;

/**
 * \brief           Return unique segment lit when \a remove & \a rest are removed from \a segments
 * \tparam Args     Types of \a rest
 * \param  segments Segments initially lit
 * \param  remove   Segment to remove from \a segments
 * \param  rest     Any more segments to remove
 * \return          Unique lit segment or '\0' if there isn't one
 */
template<typename... Args>
auto unique_segment(Segments const& segments, Segments const& remove, Args... rest) -> Segment
{
  std::string temp;
  std::set_difference(segments.begin(), segments.end(), remove.begin(), remove.end(),
                      std::back_inserter(temp));
  return unique_segment(temp, rest...);
}

template<typename... Args>
auto unique_segment(Segments const& segments, Segment remove, Args... rest) -> Segment
{
  std::string temp;
  std::set_difference(segments.begin(), segments.end(), &remove, &remove + 1,
                      std::back_inserter(temp));
  return unique_segment(temp, rest...);
}

auto main() -> int
{
  std::string line;
  unsigned total{0};

  while (std::getline(std::cin, line)) {
    /* We want to identify the digits 1, 4, 7, 8 - which is easy as they're a unique number of lit
     * segment2s.  We also want to make use of 0, 6, and 9 which all have six segments lit - but
     * we don't know which one matches which - so we keep them in a vector.
     */
    std::vector<std::string> six_segments;
    Segments one;
    Segments four;
    Segments seven;
    Segments eight;
    auto it{line.begin()};
    for (; *it != '|'; ++it) {
      if (*it == ' ') {
        continue;
      }
      std::string num;
      while (*it != ' ') {
        num += *it++;
      }
      std::sort(num.begin(), num.end());
      switch (num.size()) {
      case 2:
        one = num;
        break;
      case 3:
        seven = num;
        break;
      case 4:
        four = num;
        break;
      case 6:
        six_segments.push_back(num);
        break;
      case 7:
        eight = num;
        break;
      default:
        break;
      }
    }

    /* Now decode: Each of these contains the character that maps to the given variable name.
     *   aa
     *  b  c
     *  b  c
     *   dd
     *  e  f
     *  e  f
     *   gg
     */
    // Segment a{'\0'};
    Segment b{'\0'};
    // Segment c{'\0'};
    // Segment d{'\0'};
    Segment e{'\0'};
    Segment f{'\0'};
    Segment g{'\0'};

    /* a is easy as it's the element lit in 7 but not in 1.  */
    Segment a{unique_segment(seven, one)};

    /* The digit 9 is the only six segment digit which has one segment lit when we remove the
     * segments lit in 4 and 7.  The remaining lit segment is g. (0 and 6 will both have e and g
     * lit in this case).
     *
     * We can then detect e because it is the element lit in 8 which is not lit in 9.
     */
    for (auto const& segments : six_segments) {
      g = unique_segment(segments, four, seven);
      if (g != '\0') {
        e = unique_segment(eight, segments);  // NOLINT(readability-suspicious-call-argument)
        break;
      }
    }

    /* The digit 0 is the only six segment digit which has one segment lit which isn't in the digit
     * 7 or is e or g.  (6 and 9 have two segments in this case).  This lit segment is b.
     */
    for (auto const& segment : six_segments) {
      b = unique_segment(segment, seven, g, e);
      if (b != '\0') {
        break;
      }
    }

    /* Segment d is the remaining segment from 4 when 1 and b have been removed. */
    Segment d{unique_segment(four, one, b)};

    /* 6 is the six-segment digit with a, b, d, e, and g lit.  (0 doesn't have d lit, 9 doesn't
     * have e lit.  The remaining segment in 6 is f.
     */
    for (auto const& segment : six_segments) {
      f = unique_segment(segment, a, b, d, e, g);
      if (f != '\0') {
        break;
      }
    }

    /* And finally c is the segment in 1 which isn't f.  */
    Segment c{unique_segment(one, f)};

    /* Check everything worked (or at least found a result).  */
    assert(a != '\0');
    assert(b != '\0');
    assert(c != '\0');
    assert(d != '\0');
    assert(e != '\0');
    assert(f != '\0');
    assert(g != '\0');

    /* nums is an array of digits containing the segments which make up the display for that digit.
     */
    std::array<Segments, 10> nums{
      std::string{a, b, c, e, f, g},     // 0
      std::string{c, f},                 // 1
      std::string{a, c, d, e, g},        // 2
      std::string{a, c, d, f, g},        // 3
      std::string{b, c, d, f},           // 4
      std::string{a, b, d, f, g},        // 5
      std::string{a, b, d, e, f, g},     // 6
      std::string{a, c, f},              // 7
      std::string{a, b, c, d, e, f, g},  // 8
      std::string{a, b, c, d, f, g}      // 9
    };

    /* Sort the digit lists into alphabetical order so checking becomes easier.  */
    for (auto& num : nums) {
      std::sort(num.begin(), num.end());
    }

    /* Now work out the value for this line.  */
    unsigned value{0};
    ++it;
    while (it != line.end()) {
      if (*it == ' ') {
        ++it;
        continue;
      }
      std::string s;
      while (it != line.end() && *it != ' ') {
        s += *it++;
      }
      std::sort(s.begin(), s.end());
      auto num{std::find(nums.begin(),  // NOLINT(llvm-qualified-auto,readability-qualified-auto)
                         nums.end(), s)};
      assert(num != nums.end());
      value *= 10;
      value += num - nums.begin();
    }
    total += value;
  }

  std::cout << "Total: " << total << '\n';
  return 0;
}