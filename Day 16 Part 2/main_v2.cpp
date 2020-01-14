#include <algorithm>
#include <array>
#include <charconv>
#include <iostream>
#include <unordered_map>

#include <boost/range/irange.hpp>

#include "input.hpp"

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
  // splitting by whitespace misses the last word, so we need to adjust manually, too
  num_words /= num_lines;
  ++num_words;

  return stats;
}

constexpr auto PARSE_STATS = parse_stats(puzzle_input);

constexpr auto NUM_WORDS = PARSE_STATS.num_words - 2;
constexpr auto NUM_LINES = PARSE_STATS.num_lines;

constexpr auto NUM_AUNTS = NUM_LINES;
constexpr auto NUM_CLUES = 3;

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

auto split_line(std::string_view line, std::string_view delims = " ,:") {
  auto words = std::array<decltype(line), NUM_WORDS>{};
  decltype(line.size()) pos = 0;
  auto index = 0;
  while(pos != line.npos) {
    const auto end = line.find_first_of(delims);
    words[index++] = line.substr(0, end);
    pos = line.find_first_not_of(delims, end);
    line.remove_prefix(pos);
  }

  return words;
}

// no sorting necessary, so unordered_map it is
using Aunt = std::unordered_map<std::string_view, uintmax>;
using Aunts = std::array<Aunt, NUM_AUNTS>;

auto parse(std::string_view input) {

  auto aunts = Aunts{};
  auto index = 0;

  const auto to_int = [] (auto input) {
    uintmax value = 0;
    std::from_chars(input.begin(), input.end(), value);
    return value;
  };

  for(auto line : split_input(input)) {

    // ignore the aunt's id, we will get it from the array's index, if needed
    const auto pos = line.find(':');
    line.remove_prefix(pos + 2);

    const auto& words = split_line(line);

    auto& aunt = aunts[index++];

    for(const auto i : boost::irange(decltype(NUM_WORDS){}, NUM_WORDS, 2)) {
      auto name = words[i];
      aunt[name] = to_int(words[i+1]);
    }
  }

  return aunts;
}

auto solution(std::string_view input) {

  const auto aunts = parse(input);

  const auto sue = Aunt{
    {"children", 3},
    {"cats", 7},
    {"samoyeds", 2},
    {"pomeranians", 3},
    {"akitas", 0},
    {"vizslas", 0},
    {"goldfish", 5},
    {"trees", 3},
    {"cars", 2},
    {"perfumes", 1}
  };

  const auto equal_or = [] (auto name, auto lhs, auto rhs) {
    return (name == lhs) || (name == rhs);
  };

  const auto result = std::find_if(aunts.begin(), aunts.end(), [&] (const auto& aunt) {

    return std::all_of(aunt.begin(), aunt.end(), [&] (auto& clue) {

      auto result = false;

      const auto [name, num] = clue;
      const auto sue_num = sue.at(name);

      if(equal_or(name, "cats", "trees")) {
        result = (num > sue_num);
      } else if(equal_or(name, "pomeranians", "goldfish")) {
        result = (num < sue_num);
      } else {
        result = (num == sue_num);
      }

      return result;
    });
  });

  const auto id = (result - aunts.begin() + 1);

  return id;
}

int main() {
  std::cout << solution(puzzle_input) << std::endl;
}

