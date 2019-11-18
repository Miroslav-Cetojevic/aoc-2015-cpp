#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

template<typename S>
auto has_bad_substring(const S& string) {

	static const auto bad_strings = std::vector{ S{"ab"},
												 S{"cd"},
												 S{"pq"},
												 S{"xy"} };

	auto is_bad = std::any_of(bad_strings.begin(), bad_strings.end(), [&string] (const auto& bad_string) {
		return (string.find(bad_string) != string.npos);
	});

	return is_bad;
}

template<typename S, typename N>
auto find_vowel(const S& string, N pos) {

	auto c = string[pos];

	auto result = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');

	return (result ? 1 : 0);
}

template<typename S>
auto has_double_letters_and_triple_vowels(const S& string) {

	auto count_vowels = find_vowel(string, 0U);
	auto has_triple_vowels = false;
	auto has_double_letters = false;

	auto has_both = false;

	for(auto i = 0U, j = (i + 1);
		j < string.length() && !has_both;
		++i, ++j) {

		if(!has_double_letters) {

			has_double_letters = (string[i] == string[j]);
		}

		count_vowels += find_vowel(string, j);

		has_triple_vowels = (count_vowels >= 3);

		has_both = (has_double_letters && has_triple_vowels);
	}

	return has_both;
}

template<typename S>
auto is_nice_string(const S& string) {

	return !has_bad_substring(string)
		   && has_double_letters_and_triple_vowels(string);
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
