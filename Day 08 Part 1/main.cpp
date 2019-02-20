#include <fstream>
#include <iostream>
#include <string>

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"list.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		file.seekg(0, std::ios::end);
		auto contents = std::string{};
		contents.resize(file.tellg());
		file.seekg(0, std::ios::beg);
		file.read(contents.data(), contents.size());

		auto escsum = std::uint64_t{};
		auto escmemsum = escsum;
		auto nlsum = escsum;

		for(auto it = contents.begin(); it != contents.end(); ++it) {
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

		auto codesum = contents.size() - nlsum;
		auto memsum = codesum - (2 * nlsum) - escsum + escmemsum;
		auto result = (codesum - memsum);

		std::cout << result << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
