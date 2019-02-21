#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <string>

template<typename S>
auto has_bad_substring(const S& evaluable) {
	auto bad_strings = std::array{
		S{"ab"},
		S{"cd"},
		S{"pq"},
		S{"xy"}
	};

	auto result = std::any_of(bad_strings.begin(), bad_strings.end(), [&evaluable] (const auto& bad_string) {
		return (evaluable.find(bad_string) != evaluable.npos);
	});

	return result;
}

template<typename S, typename N>
auto find_vowel(const S& evaluable, N pos) {
	auto c = evaluable[pos];
	auto result = (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	return (result ? 1 : 0);
}

template<typename S>
auto has_double_letters_and_triple_vowels(const S& evaluable) {

	auto count_vowels = find_vowel(evaluable, 0UL);
	auto has_triple_vowels = false;
	auto has_double_letters = false;

	auto result = false;

	for(auto i = 0UL, j = (i + 1);
		j < evaluable.length() && !result;
		++i, ++j) {

		if(!has_double_letters) { has_double_letters = (evaluable[i] == evaluable[j]); }

		count_vowels += find_vowel(evaluable, j);
		has_triple_vowels = (count_vowels >= 3);


		result = (has_double_letters && has_triple_vowels);
	}

	return result;
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"strings.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto count = 0UL;

		auto line = std::string{};
		while(std::getline(file, line)) {

			if(!has_bad_substring(line)
			   && has_double_letters_and_triple_vowels(line)) {
				++count;
			}
		}

		std::cout << count << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
