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

auto get_sorted_sides_list(const Dimensions& D) {
	auto list = std::vector{D.length, D.width, D.height};
	std::sort(list.begin(), list.end());
	return list;
}

auto get_total_ribbon(std::fstream& file) {
	using iterator = std::istream_iterator<Dimensions>;

	const auto begin = iterator{file};
	const auto end   = iterator{};
	const auto init  = uint64{};

	const auto accumulator = [] (auto acc, const auto& D) {

		const auto list  = get_sorted_sides_list(D);

		const auto begin = list.begin();
		const auto end   = list.end();

		const auto init_plus = uint64{};
		const auto init_multiply = uint64{1};

		// the list is sorted, i.e. the longest side is always the last element, so
		// we can subtract that from the sum of all sides to get the two smallest sides
		const auto ribbon_present = 2 * (std::accumulate(begin, end, init_plus) - list.back());
		const auto ribbon_bow = std::accumulate(begin, end, init_multiply, std::multiplies{});

		return acc += (ribbon_present + ribbon_bow);
	};

	return std::accumulate(begin, end, init, accumulator);
}

int main() {

	const auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		std::cout << get_total_ribbon(file) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
