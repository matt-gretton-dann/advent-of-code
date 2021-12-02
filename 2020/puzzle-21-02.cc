#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

using Ingredient = std::string;
using Allergen = std::string;
using Ingredients = std::set<Ingredient>;
using Allergens = std::set<Allergen>;
using IngredientInfo = std::pair<unsigned, Allergens>;

using IngredientMap = std::map<Ingredient, IngredientInfo>;
using AllergenMap = std::map<Allergen, Ingredients>;

class IngredientParser
{
public:
  void add_recipe(std::string const& s)
  {
    auto it = s.begin();
    Ingredients i;
    while (it != s.end()) {
      auto ite = std::find(it, s.end(), ' ');
      auto ingredient = std::string(it, ite);
      while (ite != s.end() && *ite == ' ') {
        ++ite;
      }
      it = ite;
      if (ingredient == "(contains") {
        break;
      }
      else {
        i.insert(ingredient);
        auto [iit, success] = ingredients_.insert({ingredient, {1, Allergens()}});
        if (!success) {
          iit->second.first++;
        }
      }
    }

    while (it != s.end()) {
      auto ite = std::find_if(it, s.end(), [](char c) -> bool { return c == ',' || c == ')'; });
      auto allergen = std::string(it, ite);
      ++ite;
      while (ite != s.end() && *ite == ' ') {
        ++ite;
      }
      it = ite;
      auto [insert_it, success] = allergens_.insert({allergen, i});
      if (!success) {
        Ingredients a;
        std::set_intersection(i.begin(), i.end(), insert_it->second.begin(),
                              insert_it->second.end(), std::inserter(a, a.end()));
        insert_it->second = a;
      }
    }
  }

  unsigned clean_ingredients()
  {
    for (auto const& kv : allergens_) {
      auto allergen = kv.first;
      std::cout << "Allergen " << allergen << ":";
      for (auto const& i : kv.second) {
        std::cout << " " << i;
        auto it = ingredients_.find(i);
        assert(it != ingredients_.end());
        it->second.second.insert(allergen);
      }
      std::cout << "\n";
    }

    unsigned count = 0;
    for (auto const& i : ingredients_) {
      if (i.second.second.size() == 0) {
        std::cout << i.first << " is not an allergen, appears " << i.second.first << ".\n";
        count += i.second.first;
      }
    }

    return count;
  }

  std::string dangerous_ingredients()
  {
    bool changed = true;
    Ingredients i;
    while (changed) {
      changed = false;
      for (auto const& kv : allergens_) {
        if (kv.second.size() == 1) {
          auto it = kv.second.begin();
          Ingredient const& ing = *it;
          auto [iit, success] = i.insert(ing);
          if (!success) {
            continue;
          }
          changed = true;
          std::cout << "Allergen " << kv.first << " is in ingredient " << ing << "\n";
          for (auto& kv2 : allergens_) {
            if (kv2.second.size() != 1) {
              kv2.second.erase(ing);
            }
          }
        }
      }
    }

    std::string result;
    for (auto const& kv : allergens_) {
      result += ",";
      result += *kv.second.begin();
    }

    return result.substr(1);
  }

private:
  IngredientMap ingredients_;
  AllergenMap allergens_;
};

int main(void)
{
  std::string line;
  IngredientParser parser;
  while (std::getline(std::cin, line)) {
    parser.add_recipe(line);
  }

  auto clean = parser.clean_ingredients();
  std::cout << "Number of clean ingredients: " << clean << "\n";

  auto dangerous = parser.dangerous_ingredients();
  std::cout << "Dangerous ingredients: " << dangerous << "\n";
  return 0;
}