/*
 * NOTE: this solution was taken straight from
 * https://www.reddit.com/r/adventofcode/comments/3xjpp2/day_20_solutions/
 */
#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/range/counting_range.hpp>

int main() {

	const auto total_presents = 33100000;
	const auto n_presents = 11;
	const auto total_houses = (total_presents / n_presents);
	const auto max_houses = 50;

	// array with 3 mil elements will smash the stack, so vector is used instead
	// every house has at least 11 presents because the first elf visits them all
	auto houses = std::vector(total_houses, n_presents);

	// elf_id is one-based, but house_id is zero-based because it's used to access the vector
	for(const auto elf_id : boost::counting_range(2, (total_houses + 1))) {

		for(auto house_id = (elf_id - 1), houses_visited = 0;
			(houses_visited < max_houses) && (house_id < total_houses);
			house_id += elf_id, ++houses_visited) {

			houses[house_id] += (elf_id * n_presents);
		}
	}

	const auto lowest_house = std::find_if(houses.begin(), houses.end(), [total_presents] (auto house) {
		return (house >= total_presents);
	});

	// add 1 to account for zero-based indexing of vector
	const auto house_number = std::distance(houses.begin(), lowest_house) + 1;

	std::cout << house_number << std::endl;

	return 0;
}
