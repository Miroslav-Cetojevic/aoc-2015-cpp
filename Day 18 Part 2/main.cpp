#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"grid.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		constexpr auto gridlen = 102UL;
		constexpr auto gridsize = (gridlen * gridlen);

		auto steps = 100UL;

		auto grid = std::array<std::size_t, gridsize>{};

		std::string line;
		for(auto row = 1UL; std::getline(file, line); ++row) {

			for(auto col = 1UL; col <= line.size(); ++col) {
				grid[row*gridlen+col] = line[col-1] - '0';
			}
		}

		auto tmp_grid = grid;

		auto play_conways_game_of_life = [&grid, &tmp_grid, &gridlen] (auto row, auto col) {
			auto sum = grid[(row-1)*gridlen+(col-1)]
						+ grid[(row-1)*gridlen+col]
						+ grid[(row-1)*gridlen+(col+1)]
						+ grid[row*gridlen+(col-1)]
						+ grid[row*gridlen+(col+1)]
						+ grid[(row+1)*gridlen+(col-1)]
						+ grid[(row+1)*gridlen+col]
						+ grid[(row+1)*gridlen+(col+1)];

			auto pos = (row * gridlen + col);

			if((grid[pos] == 1) && (sum < 2 || sum > 3)) { tmp_grid[pos] = 0; }
			else if((grid[pos] == 0) && sum == 3) { tmp_grid[pos] = 1; }
		};

		auto limit_grid = (gridlen - 1);
		auto limit_corners = (limit_grid - 1);

		for(auto step = 0UL; step < steps; ++step) {
			// traverse the grid
			for(auto row = 1UL, col = (row + 1); col < limit_corners; ++col) {
				play_conways_game_of_life(row, col);
			}

			for(auto row = 2UL; row < limit_corners; ++row) {

				for(auto col = 1UL; col < limit_grid; ++col) {
					play_conways_game_of_life(row, col);
				}
			}

			for(auto row = limit_corners, col = 2UL; col < limit_corners; ++col) {
				play_conways_game_of_life(row, col);
			}

			grid = tmp_grid;
		}

		auto count = std::accumulate(grid.begin(), grid.end(), 0UL);

		std::cout << count << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
