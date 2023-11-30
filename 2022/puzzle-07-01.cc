//
// Created by Matthew Gretton-Dann on 07/12/2022.
//

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <regex>
#include <string>

using UInt = std::uint64_t;

struct DirectoryInfo
{
  [[nodiscard]] auto total_size() const noexcept -> UInt
  {
    UInt const file_size{std::accumulate(files_.begin(), files_.end(), UInt{0},
                                         [](UInt a, auto const& b) { return a + b.second; })};
    UInt const directory_size{
      std::accumulate(dirs_.begin(), dirs_.end(), UInt{0},
                      [](UInt a, auto const& b) { return a + b.second.total_size(); })};
    return file_size + directory_size;
  }

  auto find_dir(std::string const& name)
  {
    auto it{dirs_.find(name)};
    assert(it != dirs_.end());
    return it;
  }

  auto add_dir(std::string const& name) { dirs_.insert({name, DirectoryInfo()}); }

  auto add_file(std::string const& name, UInt size) { files_.insert({name, size}); }

  std::map<std::string, UInt> files_;
  std::map<std::string, DirectoryInfo> dirs_;
};

using DirIt = std::map<std::string, DirectoryInfo>::iterator;

auto count_dir_sizes(DirIt root, UInt max_size) -> UInt
{
  UInt result{0};
  auto root_size = root->second.total_size();
  if (root_size <= max_size) {
    result += root_size;
  }
  for (auto it = root->second.dirs_.begin(); it != root->second.dirs_.end(); ++it) {
    result += count_dir_sizes(it, max_size);
  }
  return result;
}

auto main() -> int
{
  UInt const max_size{100000};
  std::string line;

  std::list<DirIt> its;
  std::map<std::string, DirectoryInfo> dirs;
  auto root{dirs.insert({"/", DirectoryInfo()}).first};
  while (std::getline(std::cin, line) && !line.empty()) {
    if (line == "$ cd /") {
      its.clear();
      its.push_back(root);
    }
    else if (line == "$ cd ..") {
      assert(!its.empty());
      its.pop_back();
    }
    else if (line == "$ ls") {
      continue;
    }
    else if (line.substr(0, 5) == "$ cd ") {
      auto sub_dir{line.substr(5)};
      assert(!its.empty());
      auto it{its.back()->second.find_dir(sub_dir)};
      its.push_back(it);
    }
    else if (line.substr(0, 4) == "dir ") {
      its.back()->second.add_dir(line.substr(4));
    }
    else {
      static std::regex const re{R"((\d+) ([a-z]+))"};
      std::smatch m;
      if (!std::regex_search(line, m, re)) {
        std::cerr << "Unable to interpret: " << line << "\n";
        return EXIT_FAILURE;
      }
      its.back()->second.add_file(m.str(2), std::stoul(m.str(1)));
    }
  }

  std::cout << "Total sizes: " << count_dir_sizes(root, max_size) << '\n';
  return 0;
}
