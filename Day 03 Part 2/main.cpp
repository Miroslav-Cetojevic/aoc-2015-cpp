#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

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
		auto operator()(const Position& p) const {
			return get_hash(p.x) ^ get_hash(p.y);
		}
	};
}

template<typename P, typename D, typename H>
auto move(P& pos, D dir, H& houses) {
	switch(dir) {
		case '^': ++pos.y; break;
		case '>': ++pos.x; break;
		case 'v': --pos.y; break;
		case '<': --pos.x; break;
	}
	++houses[pos];
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto houses = std::unordered_map<Position, ssize_t>{};

		auto santapos = Position{};
		auto robopos = santapos;

		// Santa & Robo start at pos(0,0)
		++houses[santapos];
		++houses[robopos];

		char santadir, robodir;
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
