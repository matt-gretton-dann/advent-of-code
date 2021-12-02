#include <algorithm>
#include <iostream>
#include <string>

// Check if a string is nice:
// Nice strings have:
//   repeated double letters - but not overlapping
//   repeated letters separated by one other.
auto is_nice(std::string const& s) noexcept -> bool
{
  bool repeated_pair = false;
  bool repeated_sep = false;

  std::cout << s;
  for (std::string::size_type i = 0; i < s.length() - 2; ++i) {
    // Find whether the two characters starting at `i` are repeated in the rest of
    // the string.  We don't have to look backwards as we will already have
    // scanned it.
    if (s.substr(i + 2).find(s.substr(i, 2)) != std::string::npos) {
      repeated_pair = true;
      std::cout << " repeated(" << s.substr(i, 2) << ")";
    }
    if (s[i] == s[i + 2]) {
      repeated_sep = true;
      std::cout << " pair(" << s.substr(i, 3) << ")";
    }
  }
  if (repeated_pair && repeated_sep) {
    std::cout << " match";
  }
  std::cout << "\n";

  return repeated_pair && repeated_sep;
}

auto main() -> int
{
  unsigned nice_strings{0};
  for (std::string line; std::getline(std::cin, line);) {
    nice_strings += is_nice(line) ? 1 : 0;
  }
  std::cout << nice_strings << '\n';

  return 0;
}