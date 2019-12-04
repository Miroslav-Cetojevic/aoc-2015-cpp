/*
 * NOTE: this is a "power set" problem
 */
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/range/counting_range.hpp>

template<typename T>
auto get_vector_from_file(std::fstream& file) {
	return std::vector<T>{
		std::istream_iterator<T>{file},
		std::istream_iterator<T>{}
	};
}

int main() {

	const auto filename = std::string{"containers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		const auto containers = get_vector_from_file<std::uint64_t>(file);

		const auto target_volume = std::uint64_t{150};

		/*
		 * NOTE: the following algorithm was adapted
		 * from this stackoverflow.com answer:
		 * https://stackoverflow.com/a/19891145/699211
		 */

		/* compute power set */
		const auto size = containers.size();

		// the highest theoretical minimum
		// is all the available containers
		auto min_containers = size;

		auto powerset = std::vector<std::vector<std::uint64_t>>(1 << size);

		powerset[0] = {};

		using NumContainers = std::uint64_t;
		using Count = std::uint64_t;

		// how many times can you have exactly n containers that contain the exact volume?
		auto count_map = std::unordered_map<NumContainers, Count>{};

		for(const auto i : boost::counting_range({}, size)) {

			const auto subsize = (1 << i); // doubling size of subset

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

					const auto destsize = destination.size();

					++count_map[destsize];

					min_containers = std::min(min_containers, destsize);
				}
			}
		}

		std::cout << count_map[min_containers] << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
