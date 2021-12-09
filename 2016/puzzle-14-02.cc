//
// Created by Matthew Gretton-Dann on 06/12/2021.
//

#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <openssl/evp.h>

using MD5Digest = std::array<unsigned char, EVP_MAX_MD_SIZE>;

auto md5(std::string const& s) -> std::string
{
  MD5Digest digest;
  EVP_MD const* md{EVP_md5()};
  unsigned int md_len{0};

  EVP_MD_CTX* md_context{EVP_MD_CTX_new()};
  assert(md_context != nullptr);
  EVP_DigestInit_ex(md_context, md, nullptr);
  EVP_DigestUpdate(md_context, s.data(), s.length());
  EVP_DigestFinal_ex(md_context, digest.data(), &md_len);
  std::string result;
  static std::string_view letters{"0123456789abcdef"};
  for (auto i{0}; i < md_len; ++i) {
    result += letters[(digest[i] & 0xf0) >> 4];
    result += letters[(digest[i] & 0xf)];
  }
  return result;
}

auto key_stretch(std::string const& s) -> std::string
{
  std::string r{s};
  for (unsigned i = 0; i < 2017; ++i) {
    r = md5(r);
  }
  return r;
}

struct Cache
{
  explicit Cache(std::string const& key) : key_(key) {}

  auto md5(unsigned index) -> std::string
  {
    while (index >= cache_.size()) {
      cache_.push_back(key_stretch(key_ + std::to_string(index)));
    }
    return cache_[index];
  }

  std::string key_;
  std::vector<std::string> cache_;
};

auto has_three_char_sequence(std::string const& s) -> char
{
  std::string result;
  for (std::size_t idx{0}; idx < s.size() - 2; ++idx) {
    if (s[idx] == s[idx + 1] && s[idx] == s[idx + 2]) {
      return s[idx];
    }
  }
  return '\0';
}

auto main() -> int
{
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Failed to read line\n";
    return 1;
  }

  Cache cache{line};
  unsigned codes_seen{0};
  unsigned index{0};
  while (codes_seen != 64) {
    auto md5sum{cache.md5(index)};
    char c{has_three_char_sequence(md5sum)};
    if (c != '\0') {
      bool success{false};
      std::string s(5, c);
      for (unsigned i2 = index + 1; i2 < index + 1001; ++i2) {
        std::string code2{line};
        code2 += std::to_string(i2);
        auto md5sum2{cache.md5(i2)};
        success = md5sum2.find(s) != std::string::npos;
        if (success) {
          break;
        }
      }
      if (success) {
        std::cout << "Code " << codes_seen << " at index " << index << " letter " << c << " hash "
                  << cache.md5(index) << '\n';
        ++codes_seen;
      }
    }
    ++index;
  }

  std::cout << "Index " << index - 1 << '\n';
  return 0;
}
