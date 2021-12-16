#include <iostream>
#include <map>
#include <regex>
#include <string>

using UInt = unsigned long;

struct Layer
{
  explicit Layer(UInt depth) : depth_(depth) {}

  void advance()
  {
    pos_ += dir_;
    if (pos_ == 0 || pos_ == depth_ - 1) {
      dir_ = -dir_;
    }
  }

  [[nodiscard]] auto pos() const noexcept -> UInt { return pos_; }
  [[nodiscard]] auto depth() const noexcept -> UInt { return depth_; }

private:
  UInt pos_{0};
  UInt depth_;
  UInt dir_{1};
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

  auto severity{0};
  for (auto& layer : layers) {
    for (UInt time{0}; time != layer.first; ++time) {
      layer.second.advance();
    }
    if (layer.second.pos() == 0) {
      severity += layer.first * layer.second.depth();
    }
  }

  std::cout << "Severity: " << severity << '\n';

  return 0;
}
