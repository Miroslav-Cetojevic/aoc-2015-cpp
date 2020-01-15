#include <algorithm>
#include <array>
#include <charconv>
#include <iostream>
#include <string_view>

#include "input.hpp"

constexpr auto parse_stats(std::string_view input) {
  auto num_lines = 0;
  for(auto c : input) {
    num_lines += (c == '\n');
  }
  // last line doesn't contain newline, so we add 1
  return ++num_lines;
}

constexpr auto NUM_CONTAINERS = parse_stats(puzzle_input);
constexpr auto TARGET = 150;

using Containers = std::array<int, NUM_CONTAINERS>;

auto parse(std::string_view input) {

  const auto to_int = [] (auto input) {
    auto value = 0;
    std::from_chars(input.begin(), input.end(), value);
    return value;
  };

  auto containers = Containers{};
  auto index = 0;

  decltype(input.size()) pos = 0;

  while(pos != input.npos) {
    pos = input.find('\n');
    containers[index++] = to_int(input.substr(0, pos));
    input.remove_prefix(pos + 1);
  }
  return containers;
}

auto combinations(const Containers& containers) {

  using type = std::remove_reference_t<decltype(containers)>::value_type;

  auto kitchen = std::array<std::array<type, (NUM_CONTAINERS + 1)>, (TARGET + 1)>{};

  // dynamic programming - we keep track of how many ways a certain sum can be made with the first n elements
  kitchen.front().front() = 1;

  for(const auto& c : containers) {
    // v = volume
    for(auto v = TARGET - c; v >= 0; --v) {
      // n = number of containers
      for(auto n = NUM_CONTAINERS; n > 0; --n) {

        kitchen[v + c][n] += kitchen[v][n - 1];

      }
    }
  }

  const auto& fridge = kitchen[TARGET];

  return *std::find_if(fridge.begin(), fridge.end(), [] (const auto v) {
    return (v > 0);
  });
}

auto solution(std::string_view input) {

  const auto& containers = parse(input);

  return combinations(containers);
}

int main() {
  std::cout << solution(puzzle_input) << std::endl;
}
