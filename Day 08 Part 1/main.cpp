#include <iostream>

#include "puzzle_input.hpp"

auto solution = [] (auto input) {
	auto escsum = std::size_t{};
	auto escmemsum = escsum;
	auto nlsum = escsum;

	const auto backslash = '\\';

	for(auto it = input.begin(); it != input.end(); ++it) {
		auto c = *it;
		if(c == backslash) {
			c = *(++it);
			++escmemsum;
			if(c == backslash || c == '\"') {
				escsum += 2;
			} else {
				escsum += 4;
				it += 2;
			}
		} else if(c == '\n') {
			++nlsum;
		}
	}

	const auto codesum = input.size() - nlsum;
	const auto memsum = codesum - (2 * nlsum) - escsum + escmemsum;

	// +2 for the double quotes at beginning and end of string
	const auto result = (codesum - memsum) + 2;

	return result;
};

int main() {
	std::cout << solution(puzzle_input) << std::endl;
}
