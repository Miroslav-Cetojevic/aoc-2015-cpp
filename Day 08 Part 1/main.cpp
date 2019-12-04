#include <fstream>
#include <iostream>
#include <iterator>
#include <string>

int main() {

	const auto filename = std::string{"list.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		const auto list = std::string{
			std::istreambuf_iterator<char>{file},
			std::istreambuf_iterator<char>{}
		};

		auto escsum = unsigned{};
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
