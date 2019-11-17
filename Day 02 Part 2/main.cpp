#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>

struct Dimensions {
	unsigned length, width, height;
};

auto& operator>>(std::istream& in, Dimensions& D) {
	return in >> D.length >> D.width >> D.height;
}

int main() {

	auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto total_ribbon = 0U;

		Dimensions D;

		while(file >> D) {

			auto& l = D.length;
			auto& w = D.width;
			auto& h = D.height;

			// get the two smallest sides, min1 & min2
			if(l > w) { std::swap(l, w); }

			auto min1 = l;
			auto min2 = std::min(w, h);

			total_ribbon += 2 * (min1 + min2) + (l * w * h);
		}

		std::cout << total_ribbon << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
