#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>

// Various types that are useful
using StringSet = std::set<std::string>;
using StringMap = std::map<std::string, std::string>;
using ValidateFn = std::function<bool(std::string const &)>;
using ValidateMap = std::map<std::string, ValidateFn>;

namespace {
/** \brief      Parse a string into a map of key/value pairs.
 *  \param  str String to parse
 *  \return     Map
 *
 * \a str is expected to look like a space separated list of pairs where each
 * pair is of the form 'key:value'.
 */
StringMap parse_line(std::string const &str) {
  StringMap result;
  std::string::size_type pos = 0;

  while (pos != std::string::npos) {
    while (pos < str.length() && str.at(pos) == ' ') {
      ++pos;
    }
    auto end_pos = str.find(' ', pos);
    auto colon_pos = str.find(':', pos);
    assert(colon_pos != std::string::npos);
    assert(colon_pos < end_pos);
    auto key_len = colon_pos - pos;
    auto value_len = (end_pos == std::string::npos)
                         ? std::string::npos
                         : (end_pos - (colon_pos + 1));
    auto key = str.substr(pos, key_len);
    auto value = str.substr(colon_pos + 1, value_len);
    result.insert({key, value});
    pos = end_pos;
  }

  return result;
}

/** \brief      Validate a year
 *  \param  s   String to validate
 *  \param  min Minimum year to accept
 *  \param  max Maximum year to accept
 *  \return     True if \a s is in range [min, max]
 */
bool validate_year(std::string const &s, unsigned min, unsigned max) {
  if (s.length() != 4) {
    return false;
  }
  std::size_t pos = 0;
  auto yr = std::stoul(s, &pos, 10);
  if (pos != s.length()) {
    return false;
  }
  return yr >= min && yr <= max;
}

/// Validate byr field
bool validate_byr(std::string const &s) { return validate_year(s, 1920, 2002); }

/// Validate iyr field
bool validate_iyr(std::string const &s) { return validate_year(s, 2010, 2020); }

/// Validate eyr field
bool validate_eyr(std::string const &s) { return validate_year(s, 2020, 2030); }

/// Validate hgt field
bool validate_hgt(std::string const &s) {
  std::size_t pos = 0;
  auto hgt = std::stoul(s, &pos, 10);
  if (pos != s.length() - 2) {
    return false;
  }
  if (s[pos] == 'c' && s[pos + 1] == 'm') {
    return hgt >= 150 && hgt <= 193;
  } else if (s[pos] == 'i' && s[pos + 1] == 'n') {
    return hgt >= 59 && hgt <= 76;
  }
  return false;
}

/** \brief      Validate a string contains valid characters only
 *  \param  s   String to check
 *  \param  len Required length of string
 *  \param cs   Valid characters
 *  \return     True iff we pass validation
 */
bool validate_chars(std::string const &s, std::string::size_type len,
                    std::string const &cs) {
  if (s.length() != len) {
    return false;
  }
  for (auto c : s) {
    if (cs.find(c) == std::string::npos) {
      return false;
    }
  }

  return true;
}

/// Validate hcl field
bool validate_hcl(std::string const &s) {
  if (s.length() != 7) {
    return false;
  }
  if (s[0] != '#') {
    return false;
  }
  return validate_chars(s.substr(1), 6, "0123456789abcdef");
}

/// Validate ecl field
bool validate_ecl(std::string const &s) {
  static const StringSet valid = {"amb", "blu", "brn", "gry",
                                  "grn", "hzl", "oth"};
  return valid.find(s) != valid.end();
}

/// Validate pid field
bool validate_pid(std::string const &s) {
  return validate_chars(s, 9, "0123456789");
}

/// Validate cid field
bool validate_cid(std::string const &s) { return true; }

/// Check if a passport is valid
///
/// A passport is valid if it contains all mandatory fields, and passes
/// validation on all fields.
bool is_valid_passport(StringMap const &found) {
  static const StringSet mandatory{"byr", "iyr", "eyr", "hgt",
                                   "hcl", "ecl", "pid"};
  static const ValidateMap validators{
      {"byr", validate_byr}, {"iyr", validate_iyr}, {"eyr", validate_eyr},
      {"hgt", validate_hgt}, {"hcl", validate_hcl}, {"ecl", validate_ecl},
      {"pid", validate_pid}, {"cid", validate_cid},
  };

  unsigned mandatory_found = 0;
  for (auto const &kv : found) {
    if (mandatory.find(kv.first) != mandatory.end()) {
      ++mandatory_found;
    }
    auto validator = validators.find(kv.first);
    if (validator == validators.end()) {
      std::cout << "Found invalid key: " << kv.first << "\n";
    } else {
      auto valid = validator->second(kv.second);
      if (!valid) {
        std::cout << "Invalid value for key: " << kv.first << ": " << kv.second
                  << "\n";
        return false;
      }
    }
  }

  return mandatory_found == mandatory.size();
}
} // namespace

int main(int argc, char **argv) {

  StringMap found;
  unsigned valid = 0;
  for (std::string line; std::getline(std::cin, line);) {
    if (!line.empty()) {
      auto keys = parse_line(line);
      found.insert(keys.begin(), keys.end());
    } else {
      valid += is_valid_passport(found);
      found.clear();
    }
  }
  valid += is_valid_passport(found);

  std::cout << valid << "\n";
  return 0;
}