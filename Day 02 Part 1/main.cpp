#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

struct Dimensions { std::size_t length, width, height; };

auto& operator>>(std::istream& in, Dimensions& D) {
	return in >> D.length >> D.width >> D.height;
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto D = Dimensions{};

		auto total_paper = 0UL;

		while(file >> D) {

			auto A = D.length * D.width;
			auto B = D.width * D.height;
			auto C = D.length * D.height;

			total_paper += (2 * (A + B + C)) + std::min(std::min(A, B), C);
		}

		std::cout << total_paper << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
