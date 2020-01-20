/*
 * NOTE: This solution is based on https://www.reddit.com/r/adventofcode/comments/3xflz8/day_19_solutions/cy4etju/
 *
 * Conceptually, to get the minimum steps from molecule "e" to the medicine, we need to go backwards and
 * start with the medicine. The medicine is composed of output molecules, each for which there's an input
 * molecule. So we keep replacing each molecule in the medicine with its input until there's only molecule
 * "e" left.
 *
 * Looking at the input, there are a couple observations we can make:
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
 */
#include <array>
#include <iostream>
#include <string_view>

#include "input.hpp"

constexpr auto NEWLINE = '\n';
constexpr auto UPPERCASE_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

constexpr auto parse_stats(std::string_view input) {

  auto med_size = 0;

  auto pos = input.rfind(NEWLINE) + 1;

  while(pos != input.npos) {
    auto next = input.find_first_of(UPPERCASE_ALPHABET, (pos + 1));
    ++med_size;
    pos = next;
  }

  return med_size;
}

constexpr auto MED_SIZE = parse_stats(puzzle_input);

using Medicine = std::array<std::string_view, MED_SIZE>;

/*
 * Molecules always start with an uppercase letter. If we represent the medicine
 * as a sequence of separate molecules, it becomes easier to use the count formula
 * described above later on.
 */
#include <boost/range/irange.hpp>
auto parse_medicine(std::string_view input) {

  auto medicine = Medicine{};

  input.remove_prefix(input.rfind(NEWLINE) + 1);

  for(const auto i : boost::irange(MED_SIZE)) {
    const auto pos = input.find_first_of(UPPERCASE_ALPHABET, 1);
    medicine[i] = input.substr(0, pos);
    input.remove_prefix(pos);
  }

  return medicine;
}

auto min_steps(const Medicine& medicine) {

  auto num_steps = 0;

  constexpr auto Rn = "Rn";
  constexpr auto Ar = "Ar";
  constexpr auto Y  = "Y";

  for(auto molecule : medicine) {
    /*
     * This is the application of the formula described above:
     *
     * min_steps = `count(`X(X,X)`) - count(parentheses) - 2*count(commas) - 1` steps
     *
     * - Rn and Ar are parentheses
     * - Y is a comma
     */
    num_steps += 1 - ((molecule == Rn) or (molecule == Ar)) - (2 * (molecule == Y));
  }

  return (num_steps - 1);
}

auto solution(std::string_view input) {

  const auto& medicine = parse_medicine(input);

  return min_steps(medicine);
}

int main() {

  std::cout << solution(puzzle_input) << std::endl;

}
