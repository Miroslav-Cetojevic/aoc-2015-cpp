#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Dimensions { std::size_t length, width, height; };

auto& operator>>(std::istream& in, Dimensions& D) {
	return in >> D.length >> D.width >> D.height;
}

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"dimensions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto total_ribbon = 0UL;

		Dimensions D;
		while(file >> D) {

			auto length = D.length;
			auto width = D.width;
			auto height = D.height;

			// sorts 3 elements
			if(width < length) { std::swap(length, width); }
			if(height < width) {
				std::swap(width, height);
				if(width < length) { std::swap(length, width); }
			}

			// after sorting, length & wdith are now the two shortest sides
			total_ribbon += 2 * (length + width) + (length * width * height);
		}

		std::cout << total_ribbon << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
