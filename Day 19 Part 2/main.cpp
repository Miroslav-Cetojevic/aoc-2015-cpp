/*
 * NOTE: This solution is based on https://www.reddit.com/r/adventofcode/comments/3xflz8/day_19_solutions/cy4etju/
 *
 * Conceptually, to get the minimum steps from molecule "e" to the medicine, we need to go backwards and
 * start with the medicine. The medicine is composed of output molecules, each for which there's an input
 * molecule. So we keep replacing each molecule in the medicine with its input until there's only molecule
 * "e" left.
 *
 * Looking at the input file, there are a couple observations we can make:
 *
 * a) There are only two kinds of replacements ("|" denotes multiple replacements for the same input):
 * 1. X => XX
 * 2. X => X Rn X Ar | X Rn X Y X Ar | X Rn X Y X Y X Ar
 *
 * b) Following observation a), Rn Y Ar is equivalent to ( , )
 * - X => X(X) | X(X,X) | X(X,X,X)
 *
 * c) When you have a molecule of type XX, that is, none of Rn, Y or Ar, you can apply the first production (see a)),
 * i.e. reverse the replacement like this:
 * - XX => X
 *
 * When you have a molecule of type X(X) | X(X,X) | X(X,X,X), you can apply the second production (see a)), i.e.
 * reverse the replacement like this:
 * - X(X) | X(X,X) | X(X,X,X) => X
 *
 * Applying a production counts as one step.
 *
 * d) Repeatedly applying XX => X until there's only one molecule left takes `count(X) - 1` steps
 * - ABCDE => XCDE => XDE => XE => X
 *
 * This example produces `count(`ABCDE`) - 1` = `5 - 1` = 4 steps.
 *
 * Applying X(X) => X is similar, but `()` must be taken into account, since it increases the count.
 * This is expressed by expanding the formula: `count(`X(X)`) - count(no. of parentheses) - 1` steps. Example:
 * - A(B(C(D(E)))) => A(B(C(X))) => A(B(X)) => A(X) => X
 *
 * count(`A(B(C(D(E))))`) = 13
 * count(`(((())))`) = 8
 *
 * Result: 13 - 8 - 1 = 4 steps
 *
 * Applying X(X,X) | X(X,X,X) => X adds another variable to the count formula, the comma `,`, representing molecule Y.
 * As you can observe, each comma adds two molecules `,X` to the output. Taking this into account, we can write
 * the final formula as follows:
 *
 * `count(`X(X,X)`) - count(parentheses) - 2*count(commas) - 1` steps.
 *
 * - X(X,X) => X is expressed as `6 - 2 - 2 - 1 = 1 step`
 * - X(X,X,X) => X is expressed as `8 - 2 - 4 - 1 = 1 step`
 *
 * Once those observations are understood, the implementation becomes rather simple:
 * count all the molecules in the medicine and subtract the ones that serve as `(,)`
 * using the formula as described above.
 */
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

struct MoleculesEntry {
	std::string atom;
	std::string molecule;
};

auto& operator>>(std::istream& in, MoleculesEntry& entry) {
	std::string arrow; // garbage value
	return in >> entry.atom >> arrow >> entry.molecule;
}

int main() {

	const auto filename = std::string{"molecules.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto medicine = *(std::istream_iterator<std::string>{file});

		const auto Rn = std::string{"Rn"};
		const auto Y = std::string{"Y"};
		const auto Ar = std::string{"Ar"};

		auto molecules_set = std::unordered_set<std::string>{Rn, Y, Ar};
		auto output_molecules = std::vector<std::string>{};

		MoleculesEntry entry;

		while(file >> entry) {
			molecules_set.insert(entry.atom);
			output_molecules.push_back(entry.molecule);
		}

		// remove any registered molecules from the output
		// so we can add the remaining molecules not yet in the set
		for(auto& output : output_molecules) {

			for(const auto& molecule : molecules_set) {

				for(auto pos = output.rfind(molecule); pos != output.npos; pos = output.rfind(molecule)) {
					output.erase(pos, molecule.size());
				}
			}
		}

		// register the last remaining molecules
		molecules_set.insert(output_molecules.begin(), output_molecules.end());

		// we have all the strings we need, now we can enjoy the view(s)
		auto view_medicine = std::string_view{medicine};
		const auto view_set = std::unordered_set<std::string_view>{molecules_set.begin(), molecules_set.end()};

		auto sum_molecules = std::int64_t{};
		auto sum_rn_ar = std::int64_t{};
		auto sum_y = std::int64_t{};

		// going backwards through the medicine allows us
		// to count the molecules in a single pass
		for(auto i = medicine.size(); i > 0; --i) {

			const auto molecule = view_set.find(view_medicine.substr(i, (medicine.size() - i)));

			const auto found_molecule = (molecule != view_set.end());

			if(found_molecule) {

				if(*molecule == Rn || *molecule == Ar) {
					++sum_rn_ar;
				} else if(*molecule == Y) {
					++sum_y;
				}

				++sum_molecules;

				view_medicine.remove_suffix(molecule->size());
			}
		}

		const auto result = (sum_molecules - sum_rn_ar - (2 * sum_y) - 1);

		std::cout << result << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
