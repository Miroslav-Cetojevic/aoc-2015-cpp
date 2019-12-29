#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

using uint64 = std::uint64_t;

struct Dimensions {
	uint64 length, width, height;
};

auto& operator>>(std::istream& in, Dimensions& D) {
	char c; // throwaway input
	return in >> D.length >> c >> D.width >> c >> D.height;
}

auto get_area_list(const Dimensions& D) {
	return std::vector{
		(D.length * D.width),
		(D.width * D.height),
		(D.length * D.height)
	};
}

int main() {

	const auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		using iterator = std::istream_iterator<Dimensions>;

		const auto begin = iterator{file};
		const auto end   = iterator{};
		const auto init  = uint64{};

		const auto accumulator = [] (auto acc, const auto& D) {
			const auto list  = get_area_list(D);
			const auto begin = list.begin();
			const auto end   = list.end();
			const auto init  = uint64{};
			return acc += (2 * std::accumulate(begin, end, init) + *std::min_element(begin, end));
		};

		const auto total_paper = std::accumulate(begin, end, init, accumulator);

		std::cout << total_paper << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
