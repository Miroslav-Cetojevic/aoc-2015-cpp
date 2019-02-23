#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "flat_hash_map.hpp"

struct Sue {
	std::size_t id;
	std::map<std::size_t, std::size_t> detectables;
};

template<typename T>
auto get_hash(T t) { return std::hash<T>{}(t); }

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"sue.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto start = std::chrono::steady_clock::now();

		auto aunties = std::vector<Sue>{};

		auto tokens = std::vector<std::string>{};

		std::string line;
		while(std::getline(file, line)) {

			boost::split(tokens, line, [] (auto c) { return (c == ' '); });

			auto aunt = Sue{};
			aunt.id = std::stoul(tokens[0]);

			for(auto i = 2UL; i < tokens.size(); i += 2) {
				aunt.detectables[get_hash(tokens[i-1])] = std::stoul(tokens[i]);
			}

			aunties.emplace_back(aunt);
		}

		decltype(Sue::detectables) sue{{get_hash(std::string{"children"}), 3},
									   {get_hash(std::string{"cats"}), 7},
									   {get_hash(std::string{"samoyeds"}), 2},
									   {get_hash(std::string{"pomeranians"}), 3},
									   {get_hash(std::string{"akitas"}), 0},
									   {get_hash(std::string{"vizslas"}), 0},
									   {get_hash(std::string{"goldfish"}), 5},
									   {get_hash(std::string{"trees"}), 3},
									   {get_hash(std::string{"cars"}), 2},
									   {get_hash(std::string{"perfumes"}), 1}};

		auto result = std::find_if(aunties.begin(), aunties.end(), [&sue] (auto aunt) {
			return std::includes(sue.begin(), sue.end(), aunt.detectables.begin(), aunt.detectables.end());
		});

		auto end = std::chrono::steady_clock::now();
		std::cout << result->id << std::endl;
		std::cout << "duration: " << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
