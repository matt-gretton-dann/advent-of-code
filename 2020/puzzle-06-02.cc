#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <string>

using OccuranceMap = std::map<char, unsigned>;

struct GroupAnswers
{
  void add_answers(std::string const& a)
  {
    // Increase group size
    ++group_size_;

    // Add answers
    for (auto c : a) {
      auto it = map_.find(c);
      if (it == map_.end()) {
        map_.insert({c, 1});
      }
      else {
        ++(it->second);
      }
    }
  }

  // Count the number of answered questions answered by everyone.
  unsigned all_answer_count() const noexcept
  {
    unsigned count = 0;
    for (auto kv : map_) {
      if (kv.second == group_size_) {
        ++count;
      }
    }

    return count;
  }

  void clear()
  {
    map_.clear();
    group_size_ = 0;
  }

  OccuranceMap map_;
  unsigned group_size_ = 0;
};

int main(int argc, char** argv)
{
  GroupAnswers answers;
  unsigned count = 0;
  for (std::string line; std::getline(std::cin, line);) {
    if (!line.empty()) {
      answers.add_answers(line);
    }
    else {
      count += answers.all_answer_count();
      answers.clear();
    }
  }
  count += answers.all_answer_count();

  std::cout << "Total length = " << count << "\n";
  return 0;
}