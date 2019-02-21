#include <fstream>
#include <functional>
#include <iostream>
#include <unordered_map>

template<typename T>
struct Position { T x, y; };

template<typename T>
auto operator==(const Position<T>& lhs, const Position<T>& rhs) {
	return (lhs.x == rhs.x && lhs.y == rhs.y);
}

template<typename T>
auto get_hash(T t) { return std::hash<T>{}(t); }

namespace std {
	template<typename T>
	struct hash<Position<T>> {
		auto operator()(const Position<T>& p) const {
			return get_hash(p.x) ^ get_hash(p.y);
		}
	};
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		using ssize_t = std::ptrdiff_t;
		auto houses = std::unordered_map<Position<ssize_t>, ssize_t>{};
		auto pos = Position<ssize_t>{};
		++houses[pos]; // Santa starts at pos(0,0)

		auto direction = char{};
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
