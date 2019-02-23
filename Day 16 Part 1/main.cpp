#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

struct Sue {
	std::size_t id;
	std::unordered_map<std::string, std::size_t> detectables;
};

int main() {
	std::ios_base::sync_with_stdio(false);
	std::string filename{"sue.txt"};
	std::fstream file{filename};
	if(file.is_open()) {
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<Sue> aunties;
		std::string line;
		while(std::getline(file, line)) {
			Sue aunt;
			std::vector<std::string> tokens;
			boost::split(tokens, line, [] (const char c) {
				return (c == ' ');
			});
			aunt.id = std::stoul(tokens[0]);
			for(std::size_t i = 2; i < tokens.size(); i += 2) {
				aunt.detectables[tokens[i-1]] = std::stoul(tokens[i]);
			}
			aunties.push_back(aunt);
		}
		// find the correct aunt
		Sue sue{0, {{"children", 3},
					{"cats", 7},
					{"samoyeds", 2},
					{"pomeranians", 3},
					{"akitas", 0},
					{"vizslas", 0},
					{"goldfish", 5},
					{"trees", 3},
					{"cars", 2},
					{"perfumes", 1}}};
		std::vector<Sue> search_results;
		for(const auto& aunt : aunties) {
			bool is_potential_match = true;
			for(const auto& detectable : sue.detectables) {
				auto result = aunt.detectables.find(detectable.first);
				if(result != aunt.detectables.end() && result->second != detectable.second) {
					is_potential_match = false;
					break;
				}
			}
			if(is_potential_match) {
				search_results.push_back(aunt);
			}
		}
		auto end = std::chrono::high_resolution_clock::now();
		std::cout << search_results.front().id << std::endl;
		std::cout << "duration: " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;

	} else {
		std::cerr << "Error! Could not open " << filename << "!" << std::endl;
	}

	return 0;
}
