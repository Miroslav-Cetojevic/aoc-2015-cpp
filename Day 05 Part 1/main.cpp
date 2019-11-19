#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

auto has_bad_substring(const std::string& string) {

	static const auto bad_strings = std::vector<std::string>{ {"ab"},
															  {"cd"},
															  {"pq"},
															  {"xy"} };

	auto is_bad = std::any_of(bad_strings.begin(), bad_strings.end(), [&string] (const auto& bad_string) {
		return (string.find(bad_string) != string.npos);
	});

	return is_bad;
}

auto has_adjacent_twin_letters(const std::string& string) {

	auto result = std::adjacent_find(string.begin(), string.end());

	return (result != string.end());
}

auto has_triple_vowels(const std::string& string) {

	static constexpr auto limit = 3;
	auto count = 0;

	auto result = std::find_if(string.begin(), string.end(), [&count] (const auto& c) {

		auto has_vowel = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');

		if(has_vowel) {
			++count;
		}

		auto is_triplet = (count == limit);

		return is_triplet;
	});

	return (result != string.end());
}

auto is_nice_string(const std::string& string) {

	return !has_bad_substring(string)
		   && has_adjacent_twin_letters(string)
		   && has_triple_vowels(string);
}

int main() {

	auto filename = std::string{"strings.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto count = 0U;

		std::string line;

		while(std::getline(file, line)) {

			if(is_nice_string(line)) {
				++count;
			}
		}

		std::cout << count << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
