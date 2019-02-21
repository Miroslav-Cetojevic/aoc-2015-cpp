#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <boost/iterator/counting_iterator.hpp>

#include "flat_hash_map.hpp"

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

using Person = std::size_t;
struct SeatingPair { Person left, right; };

auto operator==(const SeatingPair& lhs, const SeatingPair& rhs) {
	return ((lhs.left == rhs.left) && (lhs.right == rhs.right))
		   || ((lhs.left == rhs.right) && (lhs.right == rhs.left));
}

struct SeatingPairEntry {
	std::string person1, person2;
	std::size_t score;
};

auto& operator>>(std::istream& in, SeatingPairEntry& entry) {
	return in >> entry.person1 >> entry.score >> entry.person2;
}

template<typename T>
auto get_hash(T t) { return std::hash<T>{}(t); }

namespace std {
	template<>
	struct hash<SeatingPair> {
		auto operator()(const SeatingPair& pair) const {
			return get_hash(pair.left) ^ get_hash(pair.right);
		}
	};
}

template<typename T>
auto factorial(T n) {
	auto range = Range<T>{2, (n + 1)};
	return std::accumulate(range.begin, range.end, T{1}, std::multiplies{});
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"happiness.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto family = ska::flat_hash_set<Person>{};

		using PairHappiness = std::ptrdiff_t;
		auto happymeter = ska::flat_hash_map<SeatingPair, PairHappiness>{};

		auto me = get_hash(std::string{"Me"});
		family.insert(me);

		auto entry = SeatingPairEntry{};
		while(file >> entry) {

			auto person1 = get_hash(entry.person1);
			auto person2 = get_hash(entry.person2);

			happymeter[{person1, person2}] += entry.score;
			happymeter[{me, person1}] = happymeter[{me, person2}] = 0;

			family.insert(person1);
			family.insert(person2);
		}

		auto seating = std::vector<Person>{family.begin(), family.end()};

		auto size = family.size();
		auto f = factorial(size);
		auto limit = (f / size) * (size - 1) - 1;

		auto max_happiness = PairHappiness{};

		do {
			// since the seating is circular, the pair [first, last] won't be
			// added in the algorithm, so we initialize the sum with it
			auto init = happymeter[{seating.front(), seating.back()}];

			auto sum = std::inner_product(seating.begin(),
										  std::prev(seating.end()),
										  std::next(seating.begin()),
										  init,
										  std::plus{},
										  [&happymeter] (auto a, auto b) { return happymeter[{a, b}];});

			max_happiness = std::max(max_happiness, sum);

		} while((limit--) > 0 && std::next_permutation(seating.begin(), seating.end()));

		std::cout << max_happiness << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
