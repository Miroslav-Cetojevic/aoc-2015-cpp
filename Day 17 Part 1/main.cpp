// NOTE: this is a "power set" problem
#include <fstream>
#include <iostream>
#include <numeric>
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
		auto count = 0UL;
		auto size = containers.size();

		auto powerset = std::vector<std::vector<std::size_t>>(1 << size);

		powerset[0] = std::vector<std::size_t>{};

		for(auto i = 0UL; i < size; ++i) {

			auto subsize = 1UL << i; // doubling size of subset

			for(auto j = 0UL; j < subsize; ++j) {

				auto& source = powerset[j];

				auto srcsize = source.size();

				auto& destination = powerset[subsize+j] = std::vector<std::size_t>(srcsize + 1);

				for(auto k = 0UL; k < srcsize; ++k) { destination[k] = source[k]; }

				destination[srcsize] = containers[i];

				auto tmp_volume = std::accumulate(destination.begin(), destination.end(), 0UL);

				if(tmp_volume == volume) { ++count; }
			}
		}

		std::cout << count << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
