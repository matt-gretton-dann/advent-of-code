#include <functional>
#include <iostream>

auto escape(std::string const& s) -> std::string
{
  std::string escaped;
  escaped += '"';
  for (auto c : s) {
    if (c == '\\' || c == '"') {
      escaped += '\\';
    }
    escaped += c;
  }
  escaped += '"';
  return escaped;
}

auto main() -> int
{
  unsigned len = 0;

  // Parse the input
  for (std::string line; std::getline(std::cin, line);) {
    std::string escaped = escape(line);
    len += escaped.length() - line.length();
    std::cout << line << ": " << line.length() << " memory bytes, " << escaped.length()
              << " escaped bytes, difference: " << escaped.length() - line.length() << "\n";
  }

  std::cout << len << "\n";

  return 0;
}