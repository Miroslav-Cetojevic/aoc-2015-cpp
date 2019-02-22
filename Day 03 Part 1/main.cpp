#include <fstream>
#include <functional>
#include <iostream>

#include "unordered_map.hpp"

using ssize_t = std::ptrdiff_t;

struct Position { ssize_t x, y; };

auto operator==(const Position& lhs, const Position& rhs) {
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}

template<typename T>
auto get_hash(T t) { return std::hash<T>{}(t); }

namespace std {
	template<>
	struct hash<Position> {
		auto operator()(const Position& pos) const {
			return get_hash(pos.x) ^ get_hash(pos.y);
		}
	};
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto houses = ska::unordered_map<Position, ssize_t>{};

		auto pos = Position{};
		++houses[pos]; // Santa starts at pos(0,0)

		char direction;
		while(file >> direction) {
			switch(direction) {
				case '^': ++pos.y; break;
				case '>': ++pos.x; break;
				case 'v': --pos.y; break;
				case '<': --pos.x; break;
			}
			++houses[pos];
		}

		std::cout << houses.size() << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
