//
// Created by Matthew Gretton-Dann on 07/12/2021.
//

#include <iostream>
#include <list>
#include <string>

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read input.\n";
    return 1;
  }
  auto n{std::stoul(line)};
  std::list<unsigned long> elves;
  for (unsigned long i{1}; i <= n; ++i) {
    elves.push_back(i);
  }
  auto it{elves.begin()};
  while (elves.size() > 1) {
    std::cout << *it << " takes ";
    ++it;
    if (it == elves.end()) {
      it = elves.begin();
    }
    std::cout << *it << '\n';
    it = elves.erase(it);
    if (it == elves.end()) {
      it = elves.begin();
    }
  }

  std::cout << "Last elf: " << elves.front() << '\n';
}