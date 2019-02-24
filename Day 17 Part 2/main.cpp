// NOTE: this is a "power set" problem
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <unordered_map>
#include <string>
#include <vector>

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"containers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto containers = std::vector<std::size_t>{};

		std::size_t value;
		while(file >> value) { containers.push_back(value); }

		/* compute power set */
		auto volume = 150UL;

		auto min_containers = std::numeric_limits<std::size_t>::max();

		auto size = containers.size();

		auto powerset = std::vector<std::vector<std::size_t>>(1UL << size);

		powerset[0] = {};

		using NumContainers = std::size_t;
		using Count = std::size_t;

		auto count_map = std::unordered_map<NumContainers, Count>{};

		for(auto i = 0UL; i < size; ++i) {

			auto subsize = 1UL << i; // doubling size of subset

			for(auto j = 0UL; j < subsize; ++j) {

				auto& source = powerset[j];

				auto srcsize = source.size();

				auto& destination = powerset[subsize+j] = std::vector<std::size_t>(srcsize + 1);

				for(auto k = 0UL; k < srcsize; ++k) { destination[k] = source[k]; }

				destination[srcsize] = containers[i];

				if(std::accumulate(destination.begin(), destination.end(), 0UL) == volume) {

					++count_map[destination.size()];

					min_containers = std::min(min_containers, destination.size());
				}
			}
		}

		std::cout << count_map[min_containers] << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
