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

constexpr auto DEFAULT_INPUT = puzzle_input;
constexpr auto DEFAULT_STEPS = 100;
constexpr auto PARSE_STATS = parse_stats(DEFAULT_INPUT);

/*
 * This will create a "ring" buffer around the actual grid, allowing us to
 * bypass the need to treat first/last rows/columns and all four corners of the
 * actual grid as special cases.
 */
constexpr auto GRIDLEN = PARSE_STATS.num_cols + 2;
constexpr auto GRIDWIDTH = PARSE_STATS.num_rows + 2;

constexpr auto GRIDSIZE = (GRIDLEN * GRIDWIDTH);

using Grid = std::array<char, GRIDSIZE>;

const auto my_index = [] (auto row, auto col) {
  return (row * GRIDLEN + col);
};

auto parse(std::string_view input) {

  auto grid = Grid{};

  for(const auto row : boost::irange(1, GRIDWIDTH-1)) {

    for(const auto col : boost::irange(1, GRIDLEN-1)) {

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

  /*
   * Each cell in the sums grid represents the sum of turned-on lights from
   * grid[i-1] to grid[i+1]. The sum of three of these cells minus the current
   * cell represents the sum of all turned-on lights in the neighborhood.
   *
   * Because we are only looking at the current cell in any iteration, we can
   * overwrite the grid in-place.
   */
  auto sums = Grid{};

  const auto turn_on_corners = [&grid, OTHERLEN = (2*GRIDLEN)] {
    // top-left corner
    grid[GRIDLEN+1] =
    // top-right corner
    grid[OTHERLEN-2] =
    // bottom-left corner
    grid[GRIDSIZE-OTHERLEN+1] =
    // bottom-right corner
    grid[GRIDSIZE-GRIDLEN-2] = 1;
  };

  turn_on_corners();

  for(const auto step : boost::irange(steps)) {

    for(const auto i : boost::irange(GRIDLEN, GRIDSIZE-GRIDLEN)) {
      sums[i] = grid[i-1]
              + grid[i]
              + grid[i+1];
    }

    for(const auto row: boost::irange(1, (GRIDWIDTH - 1))) {

      for(const auto col : boost::irange(1, (GRIDLEN - 1))) {

        const auto pos = my_index(row, col);

        auto& cell = grid[pos];

        const auto sum = sums[pos]
                       - cell
                       + sums[pos+GRIDLEN]
                       + sums[pos-GRIDLEN];

        cell = (sum == 3) or (sum == 2 and cell == 1);

      }
    }
    turn_on_corners();
  }

  return std::accumulate(grid.begin(), grid.end(), 0);
}

auto solution(std::string_view input, const unsigned steps) {

  const auto& grid = parse(input);

  const auto result = num_lights_on(grid, steps);

  return result;
}

int main() {

  std::cout << solution(DEFAULT_INPUT, DEFAULT_STEPS) << std::endl;

}
