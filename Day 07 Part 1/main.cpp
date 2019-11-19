#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

struct Position {
	unsigned row, col;
};

auto& operator>>(std::istream& in, Position& pos) {
	return in >> pos.row >> pos.col;
}

struct EndPoints {
	Position first, last;
};

auto& operator>>(std::istream& in, EndPoints& pos) {
	return in >> pos.first >> pos.last;
}

int main() {

	auto filename = std::string{"instructions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		constexpr auto gridlen = 1000U;
		constexpr auto numlights = (gridlen * gridlen);

		using Grid = std::bitset<numlights>;
		auto grid = Grid{};

		auto on = [] (auto& grid, auto pos) { grid.set(pos); };
		auto off = [] (auto& grid, auto pos) { grid.reset(pos); };
		auto toggle = [] (auto& grid, auto pos) { grid.flip(pos); };

		using cmd = void (*) (Grid&, unsigned);
		auto cmd_map = std::unordered_map<std::string, cmd>{
			{"on", on},
			{"off", off},
			{"toggle", toggle}
		};

		std::string instruction;
		EndPoints endpoints;

		while(file >> instruction >> endpoints) {

			auto current_cmd = cmd_map[instruction];

			for(auto row = endpoints.first.row; row <= endpoints.last.row; ++row) {

				for(auto col = endpoints.first.col; col <= endpoints.last.col; ++col) {

					current_cmd(grid, (row * gridlen + col));
				}
			}
		}

		std::cout << grid.count() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
