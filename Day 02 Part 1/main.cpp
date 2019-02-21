#include <algorithm>
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
		auto total_paper = 0UL;

		auto line = std::string{};
		while(std::getline(file, line)) {
			boost::split(dimensions, line, [] (char c) { return (c == 'x'); });

			auto length = std::stoul(dimensions[0]);
			auto width = std::stoul(dimensions[1]);
			auto height = std::stoul(dimensions[2]);

			auto A = length * width;
			auto B = width * height;
			auto C = length * height;

			total_paper += (2 * (A + B + C)) + std::min(std::min(A, B), C);
		}
		std::cout << total_paper << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
