#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Sue {
	std::size_t id;
	std::map<std::size_t, std::size_t> detectables;
};

struct Detectable {
	std::size_t amount;
	std::string name;
};

auto& operator>>(std::istream& in, Detectable& detectable) {
	return in >> detectable.name >> detectable.amount;
}

template<typename T>
auto get_hash(T t) { return std::hash<T>{}(t); }

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"sue.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto aunties = std::vector<Sue>{};

		auto ss = std::stringstream{};

		std::string line;
		while(std::getline(file, line)) {

			ss.str(line);

			auto aunt = Sue{};

			ss >> aunt.id;

			Detectable detectable;
			while(ss >> detectable) {
				aunt.detectables[get_hash(detectable.name)] = detectable.amount;
			}

			ss.clear();

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

		std::cout << result->id << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
