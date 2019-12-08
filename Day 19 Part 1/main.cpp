#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct MoleculesEntry {
	std::string input;
	std::string replacement;
};

auto& operator>>(std::istream& in, MoleculesEntry& entry) {
	return in >> entry.input >> entry.replacement;
}

template<typename T>
auto get_map_from_file(std::fstream& file) {

	// store all possible output molecules for each input molecule
	auto contents = std::unordered_map<std::string, std::vector<std::string>>{};

	T entry;

	while(file >> entry) {
		contents[entry.input].push_back(entry.replacement);
	}

	return contents;
}

int main() {

	const auto filename = std::string{"molecules.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		const auto medicine = *(std::istream_iterator<std::string>{file});

		const auto replacements = get_map_from_file<MoleculesEntry>(file);

		auto new_medicines = std::unordered_set<std::string>{};

		const auto med_size = medicine.size();

		for(const auto& replacement : replacements) {

			const auto& input_molecule = replacement.first;

			const auto input_size = input_molecule.size();

			for(auto i = std::uint64_t{}; i < med_size; ++i) {

				const auto pos = medicine.find(input_molecule, i);

				// if the medicine contains one of the inputs from the replacements map (as keys),
				// we can now start replacing the input with its possible outputs, creating the
				// new medicine along the way
				if(pos != medicine.npos) {

					const auto& output_molecules = replacement.second;

					for(const auto& molecule : output_molecules) {

						const auto first = (pos + input_size);
						const auto last = (med_size - input_size);

						const auto new_molecule = medicine.substr(0, pos)
												  + molecule
												  + medicine.substr(first, last);

						new_medicines.insert(new_molecule);

						i = (pos + input_size - 1);
					}
				}
			}
		}

		std::cout << new_medicines.size() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"" << std::endl;
	}

	return 0;
}
