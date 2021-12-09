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
  EVP_DigestInit_ex(md_context, md, nullptr);
  EVP_DigestUpdate(md_context, s.data(), s.length());
  EVP_DigestFinal_ex(md_context, digest.data(), &md_len);
  return md_len;
}

auto get_letter(std::string const& s) -> unsigned
{
  MD5Digest digest;
  auto len = md5(digest, s);
  assert(len >= 3);
  if (digest[0] == 0 && digest[1] == 0 && (digest[2] & 0xf0) == 0) {
    return digest[2] & 0xf;
  }
  return 16;
}

auto main() -> int
{
  constexpr std::string_view characters{"0123456789abcdef"};
  constexpr unsigned password_length{8};

  std::string result;
  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "Unable to read from stdin.\n";
    return 1;
  }

  std::uint64_t i{0};
  for (unsigned letter = 0; letter < password_length; ++letter) {
    unsigned c{0};
    while ((c = get_letter(line + std::to_string(i))) == 16) {
      ++i;
    }
    result += characters.at(c);
    ++i;
  }

  std::cout << "Password: " << result << '\n';
  return 0;
}
