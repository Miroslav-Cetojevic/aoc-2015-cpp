#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>

int main() {

	const auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		using iterator = std::istreambuf_iterator<char>;

		const auto begin = iterator{file};
		const auto end   = iterator{};

		const auto accumulator = [UP = '('] (auto floor, const auto direction) {
			return floor += (direction == UP) ? 1 : -1;
		};

		const auto result = std::accumulate(begin, end, 0, accumulator);

		std::cout << result << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
