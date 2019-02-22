#include <fstream>
#include <iostream>
#include <string>

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto floor = 0L;
		auto pos = 0UL;

		char direction;
		for(auto& i = pos; (floor >= 0) && (file >> direction); ++i) {
			floor += ((direction == '(') ? 1 : -1);
		}

		std::cout << pos << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
