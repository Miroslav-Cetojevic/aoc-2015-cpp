#include <iostream>
#include <numeric>

#include <boost/range/irange.hpp>

template<typename T = std::uint64_t>
constexpr auto get_code() {

	const T row = 3010;
	const T col = 3019;

	const T init_value = 20151125;
	const T multiplier = 252533;
	const T modulus    = 33554393;

	// the number at (row,col) represents the position
	// in the sequence of natural numbers that form
	// a triangle, as seen in the readme.txt
	const auto n = (row + col - 1);

	// this is the final number in the triangular arrangement
	// that the number at (row,col) is part of
	const auto triangular_number = ((n * (n + 1)) / 2);

	// target represents the end of the sequence we need to
	// go through in order to compute the value at (row,col).
	// this corresponds to the following mapping (see example
	// table in the readme.txt):
	// (x,y) => triangular(1,(x+y-1)) - x + 1
	const auto target = (triangular_number - row + 1);

	auto result = init_value;

	for(const auto i : boost::irange({1}, target)) {
		result *= multiplier;
		result %= modulus;
	}

	return result;
}

int main() {

	std::cout << get_code() << std::endl;

	return 0;
}
