#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>

struct Matcher {
  void add_rule(std::string const &s) {
    std::size_t len = 0;
    unsigned long id = std::stoul(s, &len);
    assert(s[len] == ':');
    ++len;
    while (s[len] == ' ') {
      ++len;
    }
    if (rules_.size() <= id) {
      rules_.resize(id + 1);
    }
    rules_[id] = s.substr(len);
  }

  void calculate_regex() {
    re42_ = expand_rule(42);
    re31_ = expand_rule(31);
  }

  bool does_match(std::string const &s) const {
    std::smatch m;
    std::string begin = "^" + re42_ + re42_;
    unsigned repeats = 1;
    while (std::regex_search(s, m, std::regex(begin))) {
      std::string end = re31_ + "$";
      std::string suffix = m.suffix();
      for (unsigned i = 0; i < repeats; ++i) {
        if (std::regex_search(suffix, std::regex("^" + end))) {
          return true;
        }
        end = re31_ + end;
      }
      begin += re42_;
      ++repeats;
    }

    return false;
  }

private:
  std::string expand_rule(std::size_t id) {
    std::string re;
    std::string const &rule = rules_[id];
    std::size_t pos = 0;
    bool needs_brackets = false;
    while (pos < rule.size()) {
      if (rule[pos] == ' ') {
        ++pos;
      } else if (rule[pos] == '|') {
        re += "|";
        needs_brackets = true;
        ++pos;
      } else if (rule[pos] == '"') {
        ++pos;
        while (pos < rule.size() && rule[pos] != '"') {
          re += rule[pos];
          ++pos;
        }
        assert(pos < rule.size());
        assert(rule[pos] == '"');
        ++pos;
      } else if (std::isdigit(rule[pos])) {
        std::size_t len = 0;
        std::size_t subid = std::stoul(rule.substr(pos), &len);
        pos += len;
        re += expand_rule(subid);
      } else {
        assert(false);
      }
    }

    if (needs_brackets) {
      re = "(" + re + ")";
    }
    std::cout << "Rule " << id << " expands to: " << re << "\n";
    return re;
  }

  std::vector<std::string> rules_;
  std::string re42_;
  std::string re31_;
};

int main(void) {
  std::string line;
  Matcher matcher;
  bool adding_rules = true;
  unsigned matches = 0;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      adding_rules = false;
      matcher.calculate_regex();
    } else if (adding_rules) {
      matcher.add_rule(line);
    } else {
      bool m = matcher.does_match(line);
      std::cout << line << ": does " << (m ? "" : "not ") << "match\n";
      matches += m;
    }
  }

  std::cout << "Total number of matches: " << matches << "\n";
  return 0;
}