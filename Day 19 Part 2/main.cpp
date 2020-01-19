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
#include <iostream>
#include <string_view>
#include <unordered_set>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/irange.hpp>

#include "input.hpp"

constexpr auto NEWLINE = '\n';

constexpr auto parse_stats(std::string_view input) {

  auto num_lines = 0;

  for(auto c : input) {
    num_lines += (c == NEWLINE);
  }

  return ++num_lines;
}

constexpr auto NUM_LINES = parse_stats(puzzle_input);

using Molecules = std::unordered_set<std::string_view>;
using Machine = std::pair<std::string_view, Molecules>;

auto parse(std::string_view input) {

  auto molecules = std::unordered_set<std::string_view>{};

  constexpr auto SPACE = ' ';

  for(const auto i : boost::irange(NUM_LINES-2)) {

    const auto linepos = input.find(NEWLINE);

    auto line = input.substr(0, linepos);

    molecules.insert(line.substr(0, line.find(SPACE)));

    auto output = line.substr((line.rfind(SPACE) + 1), line.npos);

    auto outpos = decltype(output.size()){};

    while(outpos != output.npos) {
      const auto endpos = output.find_first_of("ABCDEFGHIJKLMNOPQRSTUVWXYZ", (outpos + 1));
      molecules.insert(output.substr(outpos, (endpos - outpos)));
      outpos = endpos;
    }

    input.remove_prefix(linepos + 1);
  }
  // only one newline remains, remove it, and the input is now equal to the medicine
  input.remove_prefix(1);

  return Machine{input, molecules};
}

auto min_steps(Machine machine) {

  auto& [medicine, molecules] = machine;

  int sum_molecules = 0,
      sum_rn_ar = 0,
      sum_y = 0;

  const auto medsize = medicine.size();

  const auto reversed_range = boost::irange(medsize) | boost::adaptors::reversed;

  // going backwards through the medicine allows us to count the molecules in a single pass
  for(const auto i : reversed_range) {

    const auto molecule = molecules.find(medicine.substr(i, (medsize - i)));

    if(molecule != molecules.end()) {

      ++sum_molecules;

      const auto value = *molecule;

      sum_rn_ar += (value == "Rn" || value == "Ar");
      sum_y += (value == "Y");

      medicine.remove_suffix(molecule->size());
    }
  }

  const auto result = (sum_molecules - sum_rn_ar - (2 * sum_y) - 1);

  return result;
}

auto solution(std::string_view input) {

  auto machine = parse(input);

  return min_steps(machine);
}

int main() {

	std::cout << solution(puzzle_input) << std::endl;

}
