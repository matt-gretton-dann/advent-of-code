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

auto size_to_free(DirIt root, UInt min) -> UInt
{
  UInt result{std::numeric_limits<UInt>::max()};

  auto root_size = root->second.total_size();
  if (root_size >= min) {
    result = std::min(result, root_size);
  }
  for (auto it = root->second.dirs_.begin(); it != root->second.dirs_.end(); ++it) {
    auto t{size_to_free(it, min)};
    if (t >= min) {
      result = std::min(result, t);
    }
  }
  return result;
}

auto main() -> int
{
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

  UInt constexpr disk_size{70000000};
  UInt constexpr needed_space{30000000};
  UInt const current_space{disk_size - root->second.total_size()};
  UInt const min_space_to_free(needed_space - current_space);
  std::cout << "Size to free: " << size_to_free(root, min_space_to_free) << '\n';
  return 0;
}
