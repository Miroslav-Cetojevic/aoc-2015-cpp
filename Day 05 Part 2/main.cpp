#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

auto has_double_letter_pairs(const std::string& string) {

	static constexpr decltype(string.size()) diff = 1;

	auto range = Range{diff, string.size()}; // start search at index 1

	auto has_double_pairs = std::any_of(range.begin, range.end, [&string] (auto i) {

		auto first_pair = string.substr((i-diff), 2);
		auto second_pair = string.find(first_pair, (i+diff));

		auto has_both_pairs = (second_pair != string.npos);

		return has_both_pairs;
	});

	return has_double_pairs;
}

auto has_letter_sandwich(const std::string& string) {

	static constexpr decltype(string.size()) diff = 2;

	auto range = Range{diff, string.size()}; // start search at index 2

	auto has_sandwich = std::any_of(range.begin, range.end, [&string] (auto i) {
		return (string[i] == string[i-diff]);
	});

	return has_sandwich;
}

auto has_nice_string(const std::string& string) {

	return has_double_letter_pairs(string)
		   && has_letter_sandwich(string);
}

int main() {

	auto filename = std::string{"strings.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto count = 0;

		std::string line;

		while(std::getline(file, line)) {

			if(has_nice_string(line)) {
				++count;
			}
		}

		std::cout << count << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
