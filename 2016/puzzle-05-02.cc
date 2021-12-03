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

auto main() -> int
{
  constexpr std::string_view characters{"0123456789abcdef"};
  constexpr unsigned password_length{8};

  std::string result{"--------"};
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read from stdin.\n";
    return 1;
  }

  std::uint64_t i{0};
  while (std::any_of(result.begin(), result.end(), [](char c) { return c == '-'; })) {
    MD5Digest digest;
    auto len = md5(digest, line + std::to_string(i));
    assert(len >= 3);
    if (digest[0] == 0 && digest[1] == 0 && (digest[2] & 0xf0) == 0) {
      auto idx{digest[2] & 0xf};
      if (idx < password_length) {
        if (result.at(idx) == '-') {
          result[idx] = characters[(digest[3] & 0xf0) >> 4];
          std::cout << '\n';
        }
      }
    }
    std::cout << '\r' << result << ' ' << line << i << std::flush;
    ++i;
  }

  std::cout << "\nPassword: " << result << '\n';
  return 0;
}