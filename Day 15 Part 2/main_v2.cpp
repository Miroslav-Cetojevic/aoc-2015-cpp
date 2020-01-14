#include <algorithm>
#include <array>
#include <iostream>
#include <numeric>
#include <utility>
#include <vector>

#include <boost/range/irange.hpp>

#include "input.hpp"

using intmax = std::intmax_t;
using uintmax = std::uintmax_t;

// compile-time parsing so I can use arrays later on without magic numbers
struct ParseStats {
  uintmax num_words,
          num_lines;
};

constexpr auto SPACE = ' ';
constexpr auto NEWLINE = '\n';

constexpr auto parse_stats(std::string_view input) {
  auto stats = ParseStats{};

  auto& num_words = stats.num_words;
  auto& num_lines = stats.num_lines;

  for(auto c : input) {
    num_words += (c == SPACE);
    num_lines += (c == NEWLINE);
  }

  // there's no newline character on the last line, so we need to add it manually
  ++num_lines;
  // splitting by whitespace misses the last word, so we need to add the rest manually, too
  num_words += num_lines;

  return stats;
}

constexpr auto PARSE_STATS = parse_stats(puzzle_input);
constexpr auto NUM_WORDS = PARSE_STATS.num_words;
constexpr auto NUM_LINES = PARSE_STATS.num_lines;

constexpr auto N = 4;
constexpr auto M = 5;

using IntArray = std::array<intmax, M>;

using Ingredient = IntArray;
using Properties = IntArray;

using Ingredients = std::array<IntArray, N>;

auto split_input(std::string_view input) {
  auto lines = std::array<decltype(input), NUM_LINES>{};
  decltype(input.size()) pos = 0;
  auto index = 0;
  while(pos != input.npos) {
    pos = input.find(NEWLINE);
    lines[index++] = input.substr(0, pos);
    input.remove_prefix(pos + 1);
  }
  return lines;
}

auto split_line(std::string_view line) {
  auto words = std::array<decltype(line), NUM_WORDS>{};
  decltype(line.size()) pos = 0;
  auto index = 0;
  while(pos != line.npos) {
    pos = line.find(SPACE);
    words[index++] = line.substr(0, pos);
    line.remove_prefix(pos + 1);
  }
  return words;
}

auto to_int(std::string_view input) {
  auto result = intmax{};
  const auto negative = (input.front() == '-');
  if(negative) {
    input.remove_prefix(1);
  }
  for(auto c : input) {
    result *= 10;
    result += (c - '0');
  }
  return (negative ? -result : result);
}

auto parse(std::string_view input) {

  const auto to_int_suffix = [] (auto token) {
    token.remove_suffix(1);
    return to_int(token);
  };

  auto ingredients = Ingredients{};
  auto index = 0;

  for(const auto& line : split_input(input)) {

    const auto& words = split_line(line);

    const auto word = [&words] (auto i) {
      return words[(i + 1) * 2];
    };

    auto& ingredient = ingredients[index++];

    for(const auto i : boost::irange(N)) {
      ingredient[i] = to_int_suffix(word(i));
    }

    ingredient[N] = to_int(word(N));
  }

  return ingredients;
}

auto plus(const Properties& lhs, const Properties& rhs) {
  auto result = Properties{};
  // element-wise addition of lhs and rhs
  std::transform(lhs.begin(), lhs.end(), rhs.begin(), result.begin(), std::plus{});
  return result;
}

auto multiply(const intmax scalar, const Ingredient& ingredient) {
  auto result = Properties{};
  std::transform(ingredient.begin(), ingredient.end(), result.begin(), [scalar] (auto property) {
    return (scalar * property);
  });
  return result;
}

auto get_max_score(const Ingredients& ingredients) {

  auto max_score = intmax{};

  intmax min_spoons = 1,
         max_spoons = 100;

  const intmax calories = 500;

  for(auto fr = min_spoons; fr < (max_spoons - 2); ++fr) {

    for(auto ca = min_spoons; ca < (max_spoons - fr - 1); ++ca) {

      for(auto bu = min_spoons; bu < (max_spoons - fr - ca); ++bu) {

        const auto su = (max_spoons - fr - ca - bu);

        const auto recipe = IntArray{fr, ca, bu, su};

        const auto properties = std::inner_product(recipe.begin(), recipe.end(), ingredients.begin(), Properties{}, plus, multiply);

        if(properties.back() == calories) {

          const auto new_score = std::accumulate(properties.begin(), (properties.end() - 1), intmax{1}, [] (auto acc, auto value) {
            return acc * ((value > 0) ? value : 0);
          });

          max_score = std::max(new_score, max_score);

        }
      }
    }
  }

  return max_score;
}

auto solution(std::string_view input) {

  const auto ingredients = parse(input);

  return get_max_score(ingredients);
}

int main() {

  std::cout << solution(puzzle_input) << std::endl;

}

