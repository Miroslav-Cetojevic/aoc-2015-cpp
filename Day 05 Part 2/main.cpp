#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>

#include <boost/range/irange.hpp>

auto has_repeat_pairs(const std::string_view& str) {

	const auto range = boost::irange(str.size() - 3);

	const auto result = std::any_of(range.begin(), range.end(), [&, diff = 2] (auto i) {

		const auto first_pair  = str.substr(i, diff);
		const auto second_pair = str.find(first_pair, (i + diff));

		const auto has_both_pairs = (second_pair != str.npos);

		return has_both_pairs;
	});

	return result;
}

auto has_letter_sandwich(const std::string_view& str) {

	const auto size = str.size();

	const decltype(size) diff = 2; // start search at index 2

	const auto range = boost::irange(diff, size);

	const auto result = std::any_of(range.begin(), range.end(), [&] (auto i) {
		return (str[i] == str[i-diff]);
	});

	return result;
}

auto is_nice_string(const std::string_view& str) {

	return has_repeat_pairs(str)
		   && has_letter_sandwich(str);
}

auto count_nice_strings(std::fstream& file) {

	using iterator = std::istream_iterator<std::string>;

	const auto begin = iterator{file};
	const auto end   = iterator{};

	const auto result = std::accumulate(begin, end, 0, [] (auto acc, const auto& line) {
		return (acc + is_nice_string(line));
	});

	return result;
}

int main() {

	const auto filename = std::string{"strings.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		std::cout << count_nice_strings(file) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
