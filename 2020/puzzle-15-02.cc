#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <string>
#include <unordered_map>

using NumMap = std::unordered_map<int, int>;

int add_to_map(NumMap& nums, int num, int turn)
{
  auto [it, success] = nums.insert({num, turn});
  if (success) {
    // std::cout << "Turn " << turn << ": " << num << " (new)\n";
    return 0;
  }
  else {
    int r = turn - it->second;
    // std::cout << "Turn " << turn << ": " << num << " (previous seen turn "
    //          << it->second << " dist = " << r << ")\n";
    it->second = turn;
    return r;
  }
}

int run(std::string const& s, int num_turns)
{
  NumMap seen;
  std::size_t pos = 0;
  std::size_t len;
  int next_num = 0;
  int turn = 1;
  while (pos < s.size()) {
    int num = std::stoi(s.substr(pos), &len);
    pos += len;
    while (pos < s.size() && s[pos] == ',') {
      ++pos;
    }
    next_num = add_to_map(seen, num, turn);
    ++turn;
  }

  while (turn < num_turns) {
    next_num = add_to_map(seen, next_num, turn);
    ++turn;
  }

  return next_num;
}

int main(int argc, char** argv)
{
  std::string line;
  while (std::getline(std::cin, line)) {
    int r = run(line, 30000000);
    std::cout << "30000000th number: " << r << "\n";
  }

  return 0;
}