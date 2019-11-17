#include <fstream>
#include <iostream>
#include <string>

int main() {

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto pos = 0U;
		auto floor = 0;
		auto UP = '(';
		decltype(UP) direction;

		while((floor >= 0) && (file >> direction)) {

			if(direction == UP) {
				++floor;
			} else {
				--floor;
			}

			++pos;
		}

		std::cout << pos << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
