#include <fstream>
#include <iostream>
#include <string>

int main() {

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto floor = 0;
		auto UP = '(';
		decltype(UP) direction;

		while(file >> direction) {

			if(direction == UP) {
				++floor;
			} else {
				--floor;
			}
		}

		std::cout << floor << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
