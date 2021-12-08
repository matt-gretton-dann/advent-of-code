//
// Created by Matthew Gretton-Dann on 08/12/2021.
//

#include <array>
#include <iostream>
#include <string>
#include <vector>

auto main() -> int
{
  std::string line;
  unsigned total{0};

  while (std::getline(std::cin, line)) {
    std::vector<std::string> six_segments;
    std::string one;
    std::string four;
    std::string seven;
    std::string eight;
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

    /* Now decode: */
    char a{'.'}, b{'.'}, c{'.'}, d{'.'}, e{'.'}, f{'.'}, g{'.'};
    std::string temp;
    std::set_difference(seven.begin(), seven.end(), one.begin(), one.end(),
                        std::back_inserter(temp));
    a = temp[0];
    std::string union47;
    std::set_union(seven.begin(), seven.end(), four.begin(), four.end(),
                   std::back_inserter(union47));
    for (auto segment : six_segments) {
      temp.clear();
      std::set_difference(segment.begin(), segment.end(), union47.begin(), union47.end(),
                          std::back_inserter(temp));
      if (temp.size() == 1) {
        g = temp[0];
        temp.clear();
        std::set_difference(eight.begin(), eight.end(), segment.begin(), segment.end(),
                            std::back_inserter(temp));
        e = temp[0];
        break;
      }
    }
    std::string union7ge{seven + g + e};
    std::sort(union7ge.begin(), union7ge.end());
    for (auto segment : six_segments) {
      temp.clear();
      std::set_difference(segment.begin(), segment.end(), union7ge.begin(), union7ge.end(),
                          std::back_inserter(temp));
      if (temp.size() == 1) {
        b = temp[0];
      }
    }

    std::string union1b{one + b};
    std::sort(union1b.begin(), union1b.end());
    temp.clear();
    std::set_difference(four.begin(), four.end(), union1b.begin(), union1b.end(),
                        std::back_inserter(temp));
    d = temp[0];

    std::string unionabdeg{a, b, d, e, g};
    std::sort(unionabdeg.begin(), unionabdeg.end());
    for (auto segment : six_segments) {
      temp.clear();
      std::set_difference(segment.begin(), segment.end(), unionabdeg.begin(), unionabdeg.end(),
                          std::back_inserter(temp));
      if (temp.size() == 1) {
        f = temp[0];
      }
    }

    temp.clear();
    std::string unionf{f};
    std::set_difference(one.begin(), one.end(), unionf.begin(), unionf.end(),
                        std::back_inserter(temp));
    c = temp[0];

    std::array<std::string, 10> nums{
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

    for (auto& num : nums) {
      std::sort(num.begin(), num.end());
    }

    for (auto num : nums) {
      std::cout << num << '\n';
    }

    unsigned value{0};
    for (++it; it != line.end(); it = (it == line.end() ? it : it + 1)) {
      if (*it == ' ') {
        continue;
      }
      std::string s;
      while (it != line.end() && *it != ' ') {
        s += *it++;
      }
      std::sort(s.begin(), s.end());
      auto num{std::find(nums.begin(), nums.end(), s)};
      assert(num != nums.end());
      value *= 10;
      value += num - nums.begin();
    }
    total += value;
  }

  std::cout << "Total: " << total << '\n';
  return 0;
}