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

auto get_sorted_area_list(const Dimensions& D) {
	const auto length = D.length;
	const auto width  = D.width;
	const auto height = D.height;
	auto list = std::vector{
		(length * width),
		(width * height),
		(length * height)
	};
	sort(list.begin(), list.end());
	return list;
}

auto get_total_paper(std::fstream& file) {
	using iterator = std::istream_iterator<Dimensions>;

	const auto begin = iterator{file};
	const auto end   = iterator{};
	const auto init  = uint64{};

	const auto accumulator = [] (auto acc, const auto& D) {
		const auto list  = get_sorted_area_list(D);
		const auto begin = list.begin();
		const auto end   = list.end();
		const auto init  = uint64{};
		// list is sorted, so the first element will always be the smallest
		return acc += (2 * std::accumulate(begin, end, init) + list.front());
	};

	return std::accumulate(begin, end, init, accumulator);
}

int main() {

	const auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		std::cout << get_total_paper(file) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
