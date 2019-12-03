#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// std::include is used later in the code, which requires a sorted container,
// and I found std::map to be a very convenient tool for it
using Clues = std::map<std::string, unsigned>;

struct Sue {
	unsigned id;
	Clues clues;
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

			aunties.push_back(aunt);

			ss.clear();
		}

		static const auto sue = Clues{
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

		const auto real_sue = std::find_if(aunties.begin(), aunties.end(), [] (auto aunt) {
			return std::includes(sue.begin(), sue.end(), aunt.clues.begin(), aunt.clues.end());
		});

		// if the input file wasn't guaranteed to have the result we are looking for,
		// a comparison with the end-iterator would have been necessary before accessing
		// the object, but since this is AoC, we do have the (implicit) guarantee
		std::cout << real_sue->id << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
