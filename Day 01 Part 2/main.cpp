#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>

// short-circuiting variant of std::accumulate
template<typename T, typename Iterator, typename Predicate, typename Accumulator>
auto accumulate_if(Iterator first, Iterator last, T init, Predicate pred, Accumulator acc) {
    T result = init;
	for(; first != last && std::invoke(pred, *first); ++first) {
		result = std::invoke(acc, std::move(result), *first);
	}
    return T{std::move(result)};
}

auto get_position(std::fstream& file) {

	using iterator = std::istreambuf_iterator<char>;

	const auto begin = iterator{file};
	const auto end   = iterator{};
	const auto init  = 1;

	const auto predicate = [floor = 0, UP = '('] (const auto direction) mutable {
		floor += (direction == UP) ? 1 : -1;
		return (floor >= 0);
	};

	// the character we get from istreambuf_iterator is not needed here
	const auto accumulator = [] (auto acc, const auto) {
		return ++acc;
	};

	return accumulate_if(begin, end, init, predicate, accumulator);
}

int main() {

	const auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		const auto position = get_position(file);

		std::cout << position << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
