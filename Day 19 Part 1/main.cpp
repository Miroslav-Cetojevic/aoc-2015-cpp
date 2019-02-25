#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct MoleculeEntry {
	std::string input;
	std::string replacement;
};

auto& operator>>(std::istream& in, MoleculeEntry& entry) {
	return in >> entry.input >> entry.replacement;
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"molecules.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto replacements = std::unordered_map<std::string, std::vector<std::string>>{};

		std::string medicine;
		file >> medicine;

		MoleculeEntry entry;
		while(file >> entry) {
			replacements[entry.input].push_back(entry.replacement);
		}

		auto new_molecules = std::unordered_set<std::string>{};

		auto med_size = medicine.size();

		for(const auto& mapping : replacements) {

			auto& input = mapping.first;

			auto& possible_outputs = mapping.second;

			const auto input_size = input.size();

			for(auto i = 0UL; i < med_size; ++i) {

				const auto pos = medicine.find(input, i);

				if(pos != medicine.npos) {

					for(const auto& molecule : possible_outputs) {

						const auto replacement = medicine.substr(0, pos)
												 + molecule
												 + medicine.substr(pos + input_size,
																   med_size - input_size);

						new_molecules.insert(replacement);

						i = (pos + input_size - 1);
					}
				}
			}
		}

		std::cout << new_molecules.size() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"" << std::endl;
	}

	return 0;
}
