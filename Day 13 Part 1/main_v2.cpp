#include <algorithm>
#include <charconv>
#include <iostream>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/range/irange.hpp>

#include "input.hpp"

using intmax = std::intmax_t;
using uintmax = std::uintmax_t;

constexpr auto SPACE = ' ';
constexpr auto NEWLINE = '\n';

/*
 * compile-time parsing to allow for reserving the size of vectors
 * when taking another pass through the puzzle input
 */
constexpr auto parse_stats(std::string_view input) {
  auto num_words = uintmax{},
       num_lines = uintmax{};

  for(auto c : input) {
    num_words += (c == SPACE);
    num_lines += (c == NEWLINE);
  }

  ++num_lines;
  num_words = (num_words / num_lines) + 1;

  return std::pair{num_words, num_lines};
}

constexpr auto PARSE_STATS = parse_stats(puzzle_input);
constexpr auto NUM_WORDS = PARSE_STATS.first;
constexpr auto NUM_LINES = PARSE_STATS.second;

auto triangle(uintmax n) {
  return (n * (n - 1)) / 2;
}

template<typename T>
class AdjacencyMatrix {
  private:
    std::vector<T> data;

  public:
    AdjacencyMatrix(uintmax size) : data(triangle(size), {}) {}

    auto operator[](uintmax i) {
      return &data[triangle(i)];
    }

    auto operator[](uintmax i) const {
      return &data[triangle(i)];
    }
};

using ID = uintmax;
using Pair = std::pair<ID, ID>;

auto operator==(const Pair& lhs, const Pair& rhs) {
  return lhs.first == rhs.first
         and lhs.second == rhs.second;
}

namespace std {
  template<>
  struct hash<Pair> {
    auto operator()(const Pair& pair) const {
      const auto shift = std::numeric_limits<decltype(pair.second)>::digits / 2;
      return pair.first + (pair.second << shift);
    }
  };
}

auto minmax(const Pair& pair) {
  return std::minmax(pair.first, pair.second);
}

class Happymeter {
  private:
    AdjacencyMatrix<intmax> matrix;

  public:
    uintmax num_people;

    Happymeter(uintmax size) : matrix(size), num_people(size) {};

    auto& operator[](const Pair& pair) {
      const auto [min, max] = minmax(pair);
      return matrix[max][min];
    }

    auto operator[](const Pair& pair) const {
      const auto [min, max] = minmax(pair);
      return matrix[max][min];
    }
};

auto parse(std::string_view input) {

  using Person = std::string_view;

  auto num_people = ID{};

  using People = std::unordered_map<Person, ID>;

  auto lookup = [&, people = People{}] (auto person) mutable {
    if(people.find(person) == people.end()) {
      people[person] = num_people++;
    }
    return people.at(person);
  };

  const auto split = [] (auto input, auto delimiter, auto size) {
    auto tokens = std::vector<decltype(input)>{};
    tokens.reserve(size);
    auto pos = decltype(input.size()){};
    while(pos != input.npos) {
      pos = input.find(delimiter);
      tokens.push_back(input.substr({}, pos));
      input.remove_prefix(pos + 1);
    }
    return tokens;
  };

  const auto to_int = [] (auto input) {
    intmax result;
    std::from_chars(input.begin(), input.end(), result);
    return result;
  };

  using Score = intmax;
  auto scores = std::unordered_map<Pair, Score>{};

  for(const auto line : split(input, NEWLINE, NUM_LINES)) {
    auto words = split(line, SPACE, NUM_WORDS);
    words.back().remove_suffix(1);

    const auto person1 = lookup(words.front());
    const auto person2 = lookup(words.back());

    const auto score = to_int(words[3]);
    const auto modifier = ((words[2] == "lose") ? -1 : 1);

    scores[{person1, person2}] = (score * modifier);
  }

  auto happymeter = Happymeter{num_people};

  for(const auto [pair, score] : scores) {
    happymeter[pair] += score;
  }

  return happymeter;
}

using Seating = std::vector<ID>;

auto get_happiness(const Seating& seating, const Happymeter& happymeter) {

  /*
   * We take out the last person so we can insert them
   * where the happiness is the lowest later on
   */
  const auto begin = seating.begin();
  const auto end  = seating.end() - 1;

  const auto first = seating.front();
  const auto last  = seating.back();
  const auto next_to_last = *(end - 1);

  /*
   * a) Because the seating is circular, the people at both ends of
   *    the seating need to be included as well, but it's not easily
   *    done when iterating over the seating. So we include that pair
   *    before the start of iterations.
   *
   * b) The last person is not yet included when collecting the scores
   *    for happiness (see comment at the start of this function), so
   *    we start off with the score for the next-to-last pair
   */
  const auto penultimate_score = happymeter[{first, next_to_last}];

  auto total_score = penultimate_score;
  auto min = penultimate_score;

  std::adjacent_find(begin, end, [&] (const auto a, const auto b) {
    const auto score = happymeter[{a, b}];
    total_score += score;
    min = std::min(min, score);
    return false;
  });

  /*
   * Now that we have all the necessary scores, we can insert the last
   * person in order to get the highest potential score.
   *
   * "Inserting" includes removing the lowest happiness score, then adding
   * two scores, one for the last two people and one for the people at both
   * ends of the seating.
   */
  total_score -= min;
  total_score += happymeter[{next_to_last, last}];
  total_score += happymeter[{first, last}];

  return total_score;
}

auto solution(std::string_view input) {

  auto max = intmax{};

  const auto happymeter = parse(input);
  const auto num_people = happymeter.num_people;

  auto seating = Seating(num_people, 0);

  /*
   * - Fixing one neighbor of the first person removes rotated seatings that are equivalent
   *   (A-B-C-D == D-A-B-C)
   * - Fixing the second neighbor removes mirrored seatings, which are also equivalent
   *   (A-B-C-D == D-C-B-A)
   * - Fixing the position of the first person to the second position of the seating lets
   *   us fill up the remaining seating from the fourth position to the end through
   *   std::generate.
   * - Fixing the last person allows us to only look at permutations of the first seven seats,
   *   at which point we can optimize the happiness function to insert the last person where
   *   the happiness score is the lowest - this removes another chunk of permutations to look at
   *
   * - The effect of fixing a total of four positions of the seating before performing permutations
   *   is a huge reduction of the search space: from 8! (40'320) to [(choose 2 out of 6) * 4!] (360).
   *   That's a reduction of 99%.
   */
  const auto num_permutable_seats = num_people - 1;
  seating[1] = 0;
  seating.back() = num_permutable_seats;

  for(const auto i : boost::irange({1}, (num_permutable_seats - 1))) {
    // fixt the first neighbor of the first person
    seating[0] = i;

    for(const auto j : boost::irange((i + 1), num_permutable_seats)) {
      // fix the second neighbor of the first person
      seating[2] = j;

      const auto begin = seating.begin() + 3;
      const auto end   = seating.end() - 1;

      auto current = ID{};

      // fill the remaining seats
      std::generate(begin, end, [&] {
        while(++current == i or current == j);
        return current;
      });

      do {
        max = std::max(max, get_happiness(seating, happymeter));
      } while(std::next_permutation(begin, end));
    }
  }

  return max;
}

int main() {
  std::cout << solution(puzzle_input) << std::endl;
}
