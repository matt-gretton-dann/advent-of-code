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
#include <vector>

constexpr unsigned mod = 20201227;
constexpr unsigned initial_subject = 7;

unsigned lcg(unsigned i, unsigned subject)
{
  return ((unsigned long)i * (unsigned long)subject) % mod;
}

unsigned find_loop_length(unsigned pk)
{
  unsigned v = 1;
  unsigned loop = 0;
  while (v != pk) {
    v = lcg(v, initial_subject);
    ++loop;
  }

  return loop;
}

unsigned apply(unsigned pk, unsigned loop_size)
{
  unsigned v = 1;
  for (unsigned i = 0; i < loop_size; ++i) {
    v = lcg(v, pk);
  }

  return v;
}

int main(void)
{
  std::string line;
  std::getline(std::cin, line);
  unsigned pk1 = std::stoul(line);
  std::getline(std::cin, line);
  unsigned pk2 = std::stoul(line);
  std::cout << "Public keys: " << pk1 << " " << pk2 << "\n";
  auto l1 = find_loop_length(pk1);
  auto l2 = find_loop_length(pk2);
  std::cout << "Loop lengths: " << l1 << " " << l2 << "\n";

  auto ek1 = apply(pk1, l2);
  auto ek2 = apply(pk2, l1);

  std::cout << "Encryption key: " << ek1 << " " << ek2 << "\n";

  return 0;
}