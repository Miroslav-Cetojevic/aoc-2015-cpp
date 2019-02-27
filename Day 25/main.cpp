#include <iostream>
#include <numeric>

constexpr auto get_code() {
	auto row = 3010UL;
	auto col = 3019UL;

	auto init_value = 20151125UL;
	auto multiplier = 252533UL;
	auto modulus = 33554393UL;

	auto n = (row + col - 1);
	auto sum_formula = ((n * (n + 1)) / 2);

	// target is a triangular number:  (x,y) => (1,x+y-1) - x + 1
	auto target = (sum_formula - row + 1);
	auto result = init_value;

	auto i = 1UL;
	while((i++) < target) {
		result *= multiplier;
		result %= modulus;
	}

	return result;
}

int main() {

	std::cout << get_code() << std::endl;

	return 0;
}
