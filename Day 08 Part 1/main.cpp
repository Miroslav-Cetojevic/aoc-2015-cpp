#include <fstream>
#include <iostream>
#include <string>

int main() {

	auto filename = std::string{"list.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto list = std::string{};

		file.seekg(0, std::ios::end);
		list.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(list.data(), list.size());

		auto escsum = std::size_t{};
		auto escmemsum = escsum;
		auto nlsum = escsum;

		for(auto it = list.begin(); it != list.end(); ++it) {
			auto c = *it;
			if(c == '\n') { ++nlsum; }
			if(c == '\\') {
				c = *(++it);
				++escmemsum;
				if(c == '\\' || c == '\"') {
					escsum += 2;
				} else {
					escsum += 4;
					it += 2;
				}
			}
		}

		auto codesum = list.size() - nlsum;
		auto memsum = codesum - (2 * nlsum) - escsum + escmemsum;
		auto result = (codesum - memsum);

		std::cout << result << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
