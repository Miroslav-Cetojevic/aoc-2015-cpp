#include <fstream>
#include <iostream>

#include "unordered_map.hpp"

struct Position {
	int x, y;
};

auto operator==(const Position& lhs, const Position& rhs) {
	return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

namespace std {
	template<>
	struct hash<Position> {
		auto operator()(const Position& pos) const {
			// encode x to the first 16 bits of the hash
			// and y to the last 16 bits
			return (pos.x << 16) + pos.y;
		}
	};
}

int main() {

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		// a map allows me to easily track how many
		// times a particular house has been visited
		auto houses = ska::unordered_map<Position, int>{};

		auto pos = Position{};
		++houses[pos]; // Santa always starts at pos(0,0)

		const auto NORTH = '^';
		const auto WEST  = '>';
		const auto SOUTH = 'v';
		const auto EAST  = '<';

		char direction;

		while(file >> direction) {

			switch(direction) {
				case NORTH : ++pos.y; break;
				case WEST  : ++pos.x; break;
				case SOUTH : --pos.y; break;
				case EAST  : --pos.x; break;
				default: std::cerr << "Crap, something went wrong!" << std::endl;
			}

			++houses[pos];
		}

		std::cout << houses.size() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
