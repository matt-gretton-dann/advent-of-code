#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>

struct Classifier {
  void add_ranges(std::string const &s) {
    static const std::regex re("(.+): (\\d+)-(\\d+) or (\\d+)-(\\d+)");
    std::smatch m;
    if (std::regex_search(s, m, re)) {
      auto field = field_names_.size();
      field_names_.push_back(m.str(1));
      std::cout << m.str(1) << "valid = ";
      add_range(m.str(2), m.str(3), field);
      std::cout << ", ";
      add_range(m.str(4), m.str(5), field);
      std::cout << "\n";
    }
  }

  void add_range(std::string const &lows, std::string const &highs,
                 std::size_t field) {
    int low = std::stoi(lows);
    int high = std::stoi(highs);
    ++high;
    unsigned flag = 1 << field;
    assert(flag != 0);
    if (num_to_valid_field_.size() < high) {
      num_to_valid_field_.resize(high, 0);
    }
    std::cout << "[" << low << ", " << high << "]";
    while (low < high) {
      num_to_valid_field_[low] |= flag;
      ++low;
    }
  }

  void close() {
    idx_to_valid_field_.resize(0);
    idx_to_valid_field_.resize(field_names_.size(),
                               (1 << field_names_.size()) - 1);
  }

  int count_errors(std::string const &s) const {
    std::string::size_type pos = 0;
    int result = 0;
    while (pos < s.size()) {
      std::size_t len = 0;
      int num = std::stoi(s.substr(pos), &len);
      if (num >= num_to_valid_field_.size() || num_to_valid_field_[num] == 0) {
        result += num == 0 ? 1 : num;
        std::cout << "Invalid: " << num << "\n";
      }
      pos += len;
      while (pos < s.size() && s[pos] == ',') {
        ++pos;
      }
    }

    return result;
  }

  void mark_valid(std::string const &s) {
    std::string::size_type pos = 0;
    unsigned idx = 0;
    while (pos < s.size()) {
      std::size_t len = 0;
      int num = std::stoi(s.substr(pos), &len);
      unsigned valid_fields =
          num >= num_to_valid_field_.size() ? 0 : num_to_valid_field_[num];
      idx_to_valid_field_[idx] &= valid_fields;
      pos += len;
      while (pos < s.size() && s[pos] == ',') {
        ++pos;
      }
      assert(idx_to_valid_field_[idx] != 0);
      ++idx;
    }
  }

  void print_valid() const {
    for (std::size_t idx = 0; idx < idx_to_valid_field_.size(); ++idx) {
      std::cout << "Index " << idx << " valid for fields:";
      unsigned field = 0;
      for (auto valid_fields = idx_to_valid_field_[idx]; valid_fields != 0;
           valid_fields >>= 1) {
        if (valid_fields & 1) {
          std::cout << " " << field_names_[field];
        }
        ++field;
      }
      std::cout << "\n";
    }
  }

  void reduce() {
    bool changed = true;
    while (changed) {
      changed = false;
      for (unsigned idx = 0; idx < idx_to_valid_field_.size(); ++idx) {
        auto valid_fields = idx_to_valid_field_[idx];
        if ((valid_fields & (valid_fields - 1)) == 0) {
          std::cout << "Index " << idx << " can only be field " << valid_fields
                    << "\n";
          for (unsigned idx2 = 0; idx2 < idx_to_valid_field_.size(); ++idx2) {
            if (idx == idx2) {
              continue;
            }
            changed |= (idx_to_valid_field_[idx2] & valid_fields) != 0;
            idx_to_valid_field_[idx2] &= ~valid_fields;
          }
        }
      }
    }
  }

  unsigned long calculate_product(std::string const &s,
                                  std::string const &prefix) {
    std::vector<unsigned> values;
    std::size_t pos = 0;
    while (pos < s.size()) {
      if (s[pos] == ',') {
        ++pos;
      } else {
        std::size_t len;
        values.push_back(std::stoi(s.substr(pos), &len));
        pos += len;
      }
    }

    unsigned long product = 1;
    for (unsigned field = 0; field < field_names_.size(); ++field) {
      if (field_names_[field].substr(0, prefix.size()) == prefix) {
        unsigned idx = 0;
        while (idx < idx_to_valid_field_.size() &&
               idx_to_valid_field_[idx] != (1 << field)) {
          ++idx;
        }
        assert(idx < idx_to_valid_field_.size());
        product *= values[idx];
      }
    }

    return product;
  }

  std::vector<unsigned> num_to_valid_field_;
  std::vector<unsigned> idx_to_valid_field_;
  std::vector<std::string> field_names_;
};

enum class State { Permitted, Your, Nearby };

int main(int argc, char **argv) {
  std::string line;

  State s = State::Permitted;
  Classifier c;
  int e = 0;
  std::string our_ticket;
  while (std::getline(std::cin, line)) {
    if (line == "your ticket:") {
      s = State::Your;
    } else if (line == "nearby tickets:") {
      s = State::Nearby;
      c.close();
    } else if (line == "") {
      /* nothing */;
    } else if (s == State::Permitted) {
      c.add_ranges(line);
    } else if (s == State::Your) {
      our_ticket = line;
    } else if (s == State::Nearby) {
      auto e2 = c.count_errors(line);
      e += e2;
      if (e2 == 0) {
        c.mark_valid(line);
      }
    }
  }

  c.print_valid();
  c.reduce();
  c.print_valid();

  std::cout << "Product: " << c.calculate_product(our_ticket, "departure ")
            << "\n";
  return 0;
}