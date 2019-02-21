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

template<typename S, std::size_t diff = 2>
bool has_letter_sandwich(const S& evaluable) {
	auto range = Range{diff, evaluable.size()};

	auto result = std::any_of(range.begin, range.end, [&evaluable] (auto i) {
		return (evaluable[i] == evaluable[i-diff]);
	});

	return result;
}

template<typename S, std::size_t diff = 1>
auto has_double_letter_pairs(const S& evaluable) {
	auto range = Range{diff, evaluable.size()};

	auto result = std::any_of(range.begin, range.end, [&evaluable] (auto i) {
		return (evaluable.find(evaluable.substr(i - diff, 2), (i + 1)) != evaluable.npos);
	});

	return result;
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"strings.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto line = std::string{};
		auto count = 0UL;

		while(std::getline(file, line)) {
			if(has_double_letter_pairs(line) && has_letter_sandwich(line)) {
				++count;
			}
		}

		std::cout << count << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
