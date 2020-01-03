#include <iostream>

#include "puzzle_input.hpp"

auto solution = [] (auto input) {

	auto encsum = std::size_t{};
	auto nlsum = encsum;

	const auto backslash = '\\';

	for(auto it = input.begin(); it != input.end(); ++it) {
		auto c = *it;
		if(c == backslash) {
			c = *(++it);
			if(c == backslash || c == '\"') {
				encsum += 2;
			} else {
				++encsum;
				it += 2;
			}
		} else if(c == '\n') {
			++nlsum;
		}
	}

	/* the return expression is shortened from the following:
	 *
	 * const auto codesum = (input.size() - nlsum);
	 * const auto encodesum = codesum + (4 * nlsum) + encsum + 4;
	 * const auto result = (encodesum - codesum);
	 *
	 * a) the codesum is the sum of all characters minus the newlines, since these are
	 * 	  not counted in the code.
	 * b) the encoded list contains besides the codesum the sum of additional characters
	 *    for the encoding (encsum) as well as the newlines and the double quotes at the
	 *    beginning and end of the list (also encoded)
	 *    b1) with the encoding, newline goes from ["\n", 1 char] to ["\\\n", 4 chars]
	 *    b2) there are two double quotes not encoded in the loop, they add up to another
	 *        4 chars
	 *
	 * Taking all of the above into account, the result can be expanded to:
	 *
	 * 1. (input.size() - nlsum) + (4 * nlsum) + encsum + 4 - (input.size() - nlsum)
	 * 2. input.size() + (4 * nlsum - nlsum) + encsum + 4 - input.size() + nlsum
	 *
	 * Rearrange the operands in an equivalent fashion:
	 *
	 * 3. (input.size() - input.size()) + (4 * nlsum - nlsum + nlsum) + encsum + 4
	 *
	 * Final result:
	 *
	 * 4. (4 * nlsum) + encsum + 4
	 *
	 */
	return (4 * nlsum) + encsum + 4;
};

int main() {

	std::cout << solution(puzzle_input) << std::endl;

}
