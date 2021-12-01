#include <cassert>
#include <iostream>
#include <set>
#include <string>

using StringSet = std::set<std::string>;

namespace {
StringSet parse_line(std::string const &str) {
  StringSet result;
  std::string::size_type pos = 0;

  while (pos != std::string::npos) {
    while (pos < str.length() && str.at(pos) == ' ') {
      ++pos;
    }
    auto end_pos = str.find(' ', pos);
    auto colon_pos = str.find(':', pos);
    assert(colon_pos != std::string::npos);
    assert(colon_pos < end_pos);
    auto len = colon_pos - pos;
    result.insert(str.substr(pos, len));
    pos = end_pos;
  }
  return result;
}

const StringSet mandatory{"byr", "iyr", "eyr", "hgt", "hcl", "ecl", "pid"};
const StringSet optional{"cid"};

bool is_valid_passport(StringSet const &mandatory_found) {
  return mandatory_found.size() == mandatory.size();
}
} // namespace

int main(int argc, char **argv) {

  StringSet mandatory_found;
  StringSet optional_found;
  unsigned valid = 0;
  for (std::string line; std::getline(std::cin, line);) {
    if (!line.empty()) {
      auto keys = parse_line(line);
      for (auto const &key : keys) {
        std::cout << "Key " << key << " is ";
        if (mandatory.find(key) != mandatory.end()) {
          auto [it, success] = mandatory_found.insert(key);
          std::cout << "mandatory";
          if (!success) {
            std::cout << " (duplicate)";
          }
        } else if (optional.find(key) != optional.end()) {
          auto [it, success] = optional_found.insert(key);
          std::cout << "optional";
          if (!success) {
            std::cout << " (duplicate)";
          }
        } else {
          std::cout << "unexpected";
        }
        std::cout << "\n";
      }
    } else {
      valid += is_valid_passport(mandatory_found);
      mandatory_found.clear();
      optional_found.clear();
    }
  }
  valid += is_valid_passport(mandatory_found);

  std::cout << valid << "\n";
  return 0;
}