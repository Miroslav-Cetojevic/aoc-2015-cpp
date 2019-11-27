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

		auto encsum = unsigned{};
		auto nlsum = encsum;

		for(auto it = list.begin(); it != list.end(); ++it) {
			auto c = *it;
			if(c == '\n') { ++nlsum; }
			if(c == '\\') {
				c = *(++it);
				if(c == '\\' || c == '\"') {
					// (new)codesum will already contain the sum of original
					// escape sequence lengths, so we just add the new encodings
					encsum += 2;
				} else {
					++encsum;
					it += 2;
				}
			}
		}

		auto codesum = list.size() - nlsum;
		auto new_codesum = codesum + (4 * nlsum) + encsum;
		auto result = (new_codesum - codesum);

		std::cout << result << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
