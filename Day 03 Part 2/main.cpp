#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

template<typename T>
struct Position {
	T x;
	T y;
};

template<typename T>
auto operator==(const Position<T>& a, const Position<T>& b) {
	return (a.x == b.x && a.y == b.y);
}

namespace std {
	template<typename T>
	struct hash<Position<T>> {
		auto operator()(const Position<T>& p) const {
			return std::hash<T>()(p.x)
				   ^ std::hash<T>()(p.y);
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
		using int64 = std::int64_t;
		auto houses = std::unordered_map<Position<int64>, int64>{};
		auto santapos = Position<int64>{};
		auto robopos = santapos;

		// Santa & Robo start at pos(0,0)
		++houses[santapos];
		++houses[robopos];

		auto santadir = char{};
		auto robodir = santadir;

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
