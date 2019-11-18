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

template<typename P, typename D, typename H>
auto move(P& pos, D direction, H& houses) {

	constexpr auto NORTH = '^';
	constexpr auto WEST  = '>';
	constexpr auto SOUTH = 'v';
	constexpr auto EAST  = '<';

	switch(direction) {
		case NORTH : ++pos.y; break;
		case WEST  : ++pos.x; break;
		case SOUTH : --pos.y; break;
		case EAST  : --pos.x; break;
		default: std::cerr << "Crap, something went wrong!" << std::endl;
	}

	++houses[pos];
}

int main() {

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		// a map allows me to easily track how many
		// times a particular house has been visited
		auto houses = ska::unordered_map<Position, int>{};

		auto santapos = Position{};
		auto robopos = santapos;

		// Santa & Robo start at pos(0,0)
		++houses[santapos];
		++houses[robopos];

		char santadir;
		char robodir;

		while(file >> santadir >> robodir) {
			move(santapos, santadir, houses);
			move(robopos, robodir, houses);
		}

		std::cout << houses.size() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
