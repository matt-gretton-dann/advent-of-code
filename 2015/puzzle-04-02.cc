#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <string>

#include <openssl/evp.h>

using MD5Digest = std::array<unsigned char, EVP_MAX_MD_SIZE>;

auto md5(MD5Digest& digest, std::string const& s) -> unsigned int
{
  EVP_MD const* md{EVP_md5()};
  unsigned int md_len{0};

  EVP_MD_CTX* md_context{EVP_MD_CTX_new()};
  assert(md_context != nullptr);
  EVP_DigestInit_ex2(md_context, md, nullptr);
  EVP_DigestUpdate(md_context, s.data(), s.length());
  EVP_DigestFinal_ex(md_context, digest.data(), &md_len);
  return md_len;
}

auto is_valid(std::string const& s) -> bool
{
  MD5Digest digest;
  auto len = md5(digest, s);
  assert(len >= 3);
  return digest[0] == 0 && digest[1] == 0 && digest[2] == 0;
}

auto main() -> int
{
  for (std::string line; std::getline(std::cin, line);) {
    unsigned i = 0;
    while (!is_valid(line + std::to_string(i))) {
      ++i;
    }
    std::cout << "Value = " << i << "\n";
  }

  return 0;
}