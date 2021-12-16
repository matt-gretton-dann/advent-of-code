#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>

using UInt = unsigned long;

struct Layer
{
  explicit Layer(UInt depth) : depth_(depth) {}

  [[nodiscard]] auto pos_at(UInt time) const noexcept -> UInt
  {
    time %= 2 * (depth_ - 1);
    if (time < depth_ - 1) {
      return time;
    }
    else {
      return 2 * (depth_ - 1) - time;
    }
  }

  [[nodiscard]] auto depth() const noexcept -> UInt { return depth_; }

private:
  UInt depth_;
};

using LayerMap = std::map<UInt, Layer>;

auto main() -> int
{
  static std::regex line_re{"(\\d+): (\\d+)"};
  std::string line;
  LayerMap layers;

  while (std::getline(std::cin, line) && !line.empty()) {
    std::smatch m;
    if (!std::regex_search(line, m, line_re)) {
      std::cerr << "Unable to match: " << line << '\n';
      return 1;
    }
    layers.insert({std::stoul(m.str(1)), Layer{std::stoul(m.str(2))}});
  }

  UInt offset{0};
  UInt cont{true};
  while (cont) {
    cont = false;
    for (auto& layer : layers) {
      if (layer.second.pos_at(layer.first + offset) == 0) {
        ++offset;
        cont = true;
        break;
      }
    }
  }

  std::cout << "Offset: " << offset << '\n';

  return 0;
}
