#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto dimensions = std::vector<std::string>{3};
		auto total_ribbon = 0UL;

		auto line = std::string{};
		while(std::getline(file, line)) {
			boost::split(dimensions, line, [] (char c) { return (c == 'x'); });

			auto length = std::stoul(dimensions[0]);
			auto width = std::stoul(dimensions[1]);
			auto height = std::stoul(dimensions[2]);

			// sorts 3 elements
			if(width < length) { std::swap(length, width); }
			if(height < width) {
				std::swap(width, height);
				if(width < length) { std::swap(length, width); }
			}

			// after sorting, length & wdith are now the two shortest sides
			total_ribbon += 2 * (length + width) + (length * width * height);
		}

		std::cout << "Total ribbon: " << total_ribbon << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
