#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct Sue {
	unsigned id;
	std::map<std::string, unsigned> clues;
};

struct Clue {
	std::string name;
	unsigned amount;
};

auto& operator>>(std::istream& in, Clue& clue) {
	return in >> clue.name >> clue.amount;
}

int main() {

	const auto filename = std::string{"aunts.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto aunties = std::vector<Sue>{};

		auto ss = std::stringstream{};

		std::string line;

		while(std::getline(file, line)) {

			ss.str(line);

			auto aunt = Sue{};

			ss >> aunt.id;

			Clue clue;

			while(ss >> clue) {
				aunt.clues[clue.name] = clue.amount;
			}

			ss.clear();

			aunties.push_back(aunt);
		}

		const decltype(Sue::clues) sue{
			{"children", 3},
			{"cats", 7},
			{"samoyeds", 2},
			{"pomeranians", 3},
			{"akitas", 0},
			{"vizslas", 0},
			{"goldfish", 5},
			{"trees", 3},
			{"cars", 2},
			{"perfumes", 1}
		};

		const auto result = std::find_if(aunties.begin(), aunties.end(), [&sue] (auto aunt) {
			return std::includes(sue.begin(), sue.end(), aunt.clues.begin(), aunt.clues.end());
		});

		std::cout << result->id << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
