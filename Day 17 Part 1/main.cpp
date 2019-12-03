/*
 * NOTE: this is a "power set" problem
 */
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <boost/range/counting_range.hpp>

int main() {

	const auto filename = std::string{"containers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto containers = std::vector<std::uint64_t>{};

		std::uint64_t container;

		while(file >> container) {
			containers.push_back(container);
		}

		const auto target_volume = std::uint64_t{150};

		auto count = std::uint64_t{};

		const auto size = containers.size();

		/*
		 * NOTE: the following algorithm was adapted
		 * from this stackoverflow.com answer:
		 * https://stackoverflow.com/a/19891145/699211
		 */

		/* compute power set */
		auto powerset = std::vector<std::vector<std::uint64_t>>(1 << size);

		powerset[0] = {};

		for(const auto i : boost::counting_range({}, size)) {

			const auto subsize = std::uint64_t{1} << i; // doubling size of subset

			for(const auto j : boost::counting_range({}, subsize)) {

				const auto& source = powerset[j];

				const auto srcsize = source.size();

				auto& destination = powerset[subsize+j] = std::vector<std::uint64_t>(srcsize + 1);

				for(const auto k : boost::counting_range({}, srcsize)) {
					destination[k] = source[k];
				}

				destination[srcsize] = containers[i];

				const auto new_volume = std::accumulate(destination.begin(), destination.end(), std::uint64_t{});

				if(new_volume == target_volume) {
					++count;
				}
			}
		}

		std::cout << count << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
