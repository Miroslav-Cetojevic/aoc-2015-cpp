#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

struct MoleculesEntry {
	std::string atom;
	std::string molecule;
};

auto& operator>>(std::istream& in, MoleculesEntry& entry) {
	return in >> entry.atom >> entry.molecule;
}

// NOTE: solution taken from https://www.reddit.com/r/adventofcode/comments/3xflz8/day_19_solutions/cy4etju/
int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"molecules.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto Y = std::string{"Y"};
		auto Rn = std::string{"Rn"};
		auto Ar = std::string{"Ar"};

		auto atoms = std::unordered_set<std::string>{Y, Rn, Ar};
		auto molecules = std::vector<std::string>{};

		std::string medicine;
		file >> medicine;

		MoleculesEntry entry;
		while(file >> entry) {
			atoms.insert(entry.atom);
			molecules.push_back(entry.molecule);
		}

		//erase any already registered atoms from the molecules
		for(auto& molecule : molecules) {

			for(const auto& atom : atoms) {

				for(auto pos = molecule.find(atom); pos != molecule.npos; pos = molecule.find(atom)) {
					molecule.erase(pos, atom.size());
				}
			}
		}

		std::sort(molecules.begin(), molecules.end());

		molecules.erase(std::unique(molecules.begin(), molecules.end()), molecules.end());

		// get rid of remaining empty string
		molecules.erase(std::find(molecules.begin(), molecules.end(), ""));

		// add missing atoms to the set
		atoms.insert(molecules.begin(), molecules.end());

		auto sum_atoms = 0UL;
		auto sum_rnar = 0UL;
		auto sum_y = 0UL;

		// count the atoms to calculate the eventual result
		auto i = static_cast<std::ptrdiff_t>(medicine.size());
		while((i--) > 0) {

			auto atom = atoms.find(medicine.substr(i, medicine.size() - i));

			if(atom != atoms.end()) {

				if(*atom == Rn || *atom == Ar) { ++sum_rnar; }
				else if(*atom == Y) { ++sum_y; }

				++sum_atoms;

				medicine.erase(i, atom->size());
			}
		}

		auto result = (sum_atoms - sum_rnar - (2 * sum_y) - 1);

		std::cout << result << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
