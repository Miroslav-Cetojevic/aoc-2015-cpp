#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>

#include <boost/range/counting_range.hpp>

int main() {

	const auto filename = std::string{"grid.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		/*
		 * this will create a "ring" buffer around the actual grid
		 * allowing to bypass the need to treat first/last rows,
		 * columns, and all four corners of the actual grid
		 * in a special way
		 */
		constexpr auto gridlen = std::uint64_t{102};
		constexpr auto gridsize = (gridlen * gridlen);

		auto grid = std::array<std::uint64_t, gridsize>{};

		std::string line;

		const auto index = [gridlen] (auto row, auto col) {
			return (row * gridlen + col);
		};

		// to convert a character to a number in the array
		auto char_map = std::unordered_map<char, std::uint64_t>{
			{'.', 0},
			{'#', 1}
		};

		for(auto row = std::uint64_t{1}; std::getline(file, line); ++row) {

			for(auto col : boost::counting_range({1}, (line.size() + 1))) {

				grid[index(row,col)] = char_map[line[col-1]];

			}
		}

		const auto must_turn_off = [&grid] (auto pos, auto sum) {
			return (grid[pos] == 1) && (sum < 2 || sum > 3);
		};

		const auto must_turn_on = [&grid] (auto pos, auto sum) {
			return (grid[pos] == 0) && (sum == 3);
		};

		auto tmp_grid = grid;

		constexpr auto steps = std::uint64_t{100};

		constexpr auto limit = (gridlen - 1);

		for(const auto step : boost::counting_range({}, steps)) {

			for(const auto row: boost::counting_range({1}, limit)) {

				for(const auto col : boost::counting_range({1}, limit)) {

					const auto sum = grid[(row-1)*gridlen+(col-1)]
									 + grid[(row-1)*gridlen+col]
									 + grid[(row-1)*gridlen+(col+1)]
									 + grid[row*gridlen+(col-1)]
									 + grid[row*gridlen+(col+1)]
									 + grid[(row+1)*gridlen+(col-1)]
									 + grid[(row+1)*gridlen+col]
									 + grid[(row+1)*gridlen+(col+1)];

					const auto pos = index(row, col);

					if(must_turn_off(pos, sum)) {
						tmp_grid[pos] = 0;
					} else if(must_turn_on(pos, sum)) {
						tmp_grid[pos] = 1;
					}
				}
			}

			grid = tmp_grid;
		}

		const auto count = std::accumulate(grid.begin(), grid.end(), std::uint64_t{});

		std::cout << count << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
