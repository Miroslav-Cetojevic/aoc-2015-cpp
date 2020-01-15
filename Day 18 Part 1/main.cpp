#include <array>
#include <iostream>
#include <numeric>
#include <string_view>

#include <boost/range/irange.hpp>

#include "input.hpp"

// compile-time parsing of string_view
struct ParseStats {
  int num_cols,
      num_rows;
};

constexpr auto NEWLINE = '\n';

constexpr auto parse_stats(std::string_view input) {
  auto stats = ParseStats{};

  auto& num_cols = stats.num_cols;
  auto& num_rows = stats.num_rows;

  for(const auto c : input) {
    num_cols += (c != NEWLINE);
    num_rows += (c == NEWLINE);
  }

  ++num_rows;
  num_cols /= num_rows;

  return stats;
}

constexpr auto PARSE_STATS = parse_stats(puzzle_input);

constexpr auto NUM_COLS = PARSE_STATS.num_cols;
constexpr auto NUM_ROWS = PARSE_STATS.num_rows;

constexpr auto GRIDLEN = NUM_COLS + 2;
constexpr auto GRIDSIZE = (GRIDLEN * GRIDLEN);

using Grid = std::array<char, GRIDSIZE>;

const auto my_index = [] (auto row, auto col) {
  return (row * GRIDLEN + col);
};

auto parse(std::string_view input) {

  auto grid = Grid{};

  for(const auto row : boost::irange(1, NUM_ROWS+1)) {

    for(const auto col : boost::irange(1, NUM_COLS+1)) {

      if(input.front() == '#') {
        grid[my_index(row, col)] = 1;
      }

      input.remove_prefix(1);
    }
    // skip newline
    input.remove_prefix(1);
  }

  return grid;
}

auto num_lights_on(Grid grid, const unsigned steps) {

  auto tmp_grid = grid;

  constexpr auto limit = (GRIDLEN - 1);

  for(const auto step : boost::irange(steps)) {

    for(const auto row: boost::irange(1, limit)) {

      for(const auto col : boost::irange(1, limit)) {

        const auto sum = grid[my_index(row-1,col-1)]
                       + grid[my_index(row-1,col)]
                       + grid[my_index(row-1,col+1)]
                       + grid[my_index(row,col-1)]
                       + grid[my_index(row,col+1)]
                       + grid[my_index(row+1,col-1)]
                       + grid[my_index(row+1,col)]
                       + grid[my_index(row+1,col+1)];

        const auto pos = my_index(row, col);

        const auto must_turn_off = [&] () {
          return (grid[pos]) && (sum < 2 || sum > 3);
        };

        const auto must_turn_on = [&] () {
          return (not grid[pos]) && (sum == 3);
        };

        if(must_turn_off()) {
          tmp_grid[pos] = 0;
        } else if(must_turn_on()) {
          tmp_grid[pos] = 1;
        }
      }
    }

    grid = tmp_grid;
  }

  return std::accumulate(grid.begin(), grid.end(), 0);
}

auto solution(std::string_view input, const unsigned steps) {

  const auto& grid = parse(input);

  return num_lights_on(grid, steps);
}

int main() {
  std::cout << solution(puzzle_input, 100) << std::endl;
}
