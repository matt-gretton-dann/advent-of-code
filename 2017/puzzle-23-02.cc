#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <variant>
#include <vector>

using Int = long;

auto main() -> int
{
  Int a{1};
  Int b{0};
  Int c{0};
  Int d{0};
  Int e{0};
  Int f{0};
  Int g{0};
  Int h{0};

  b = 107'900;
  c = b + 17'000;
  do {
    d = 2;
    do {
      e = b / d;
      if (d * e == b) {
        ++h;
        break;
      }
      ++d;
    } while (d != b);
    b += 17;
  } while (b <= c);

  std::cout << "h = " << h << '\n';
  return 0;
}
