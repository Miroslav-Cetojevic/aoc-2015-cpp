#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <string_view>

auto has_bad_substring(const std::string_view& str) {

	// bad strings are "ab", "cd", "pq" and "xy", and we can see that the
	// 2nd letter in each of them is always ahead by one in the alphabet
	const auto predicate = [bad = std::string_view{"acpx"}] (auto a, auto b) {
		return (b == (a + 1)) && (bad.find(a) != bad.npos);
	};

	const auto result = std::adjacent_find(str.begin(), str.end(), predicate);

	return (result != str.end());
}

auto has_adjacent_twin_letters(const std::string_view& str) {

	const auto result = std::adjacent_find(str.begin(), str.end());

	return (result != str.end());
}

auto has_triple_vowels(const std::string_view& str) {

	const auto vowels = std::string_view{"aeiou"};

	const auto predicate = [&vowels, count = 0] (const auto c) mutable {
		count += (vowels.find(c) != vowels.npos);

		return (count >= 3);
	};

	const auto result = std::find_if(str.begin(), str.end(), predicate);

	return (result != str.end());
}

auto is_nice_string(const std::string_view& str) {

	return !has_bad_substring(str)
		   && has_adjacent_twin_letters(str)
		   && has_triple_vowels(str);
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
