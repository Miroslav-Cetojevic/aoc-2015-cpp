#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
class Range {
	private:
		boost::counting_iterator<T, boost::use_default, T> begin_, end_;
	public:
		Range(T b, T e): begin_(b), end_(e) {}
		auto begin() const { return begin_; }
		auto end() const { return end_; }
};

using PersonID = unsigned;

struct SeatingPair {
	PersonID left_id;
	PersonID right_id;
};

auto operator==(const SeatingPair& lhs, const SeatingPair& rhs) {
	return ((lhs.left_id == rhs.left_id) && (lhs.right_id == rhs.right_id))
		   || ((lhs.left_id == rhs.right_id) && (lhs.right_id == rhs.left_id));
}

using Person = std::string;

struct SeatingPairEntry {
	Person left_person;
	Person right_person;
	unsigned score;
};

auto& operator>>(std::istream& in, SeatingPairEntry& entry) {
	return in >> entry.left_person >> entry.score >> entry.right_person;
}

namespace std {
	template<>
	struct hash<SeatingPair> {
		auto operator()(const SeatingPair& pair) const {
			const auto [min, max] = std::minmax(pair.left_id, pair.right_id);
			return (min * 10) + max;
		}
	};
}

template<typename T>
auto factorial(T n) {
	auto range = Range<T>{2, (n + 1)};
	return std::accumulate(range.begin(), range.end(), T{1}, std::multiplies{});
}

int main() {

	const auto filename = std::string{"happiness.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		// std::set is used for its automatic ordering of keys,
		// which is important when iterating through permutations
		// of seating orders later on
		auto family_set = std::set<PersonID>{};
		auto family_map = std::unordered_map<Person, PersonID>{};

		using PairHappiness = int;
		auto happymeter = std::unordered_map<SeatingPair, PairHappiness>{};

		const auto add_family_member = [&family_map, &family_set] (const auto& member) {

			const auto is_new = family_map.insert({member, {}}).second;

			if(is_new) {
				const auto size = family_map.size();
				family_map[member] = size;
				family_set.insert(size);
			}
		};

		SeatingPairEntry entry;

		while(file >> entry) {

			const auto& left_person = entry.left_person;
			const auto& right_person = entry.right_person;

			add_family_member(left_person);
			add_family_member(right_person);

			const auto left_id = family_map[left_person];
			const auto right_id = family_map[right_person];

			happymeter[{left_id, right_id}] += entry.score;
		}

		// elements must be sorted, fortunately std::set has already done that job for us
		auto seating = std::vector<PersonID>{family_set.begin(), family_set.end()};

		// only a subset of all possible permutations of seating orders is relevant for us,
		// so we create an upper limit to count down from when looping through permutations
		const auto size = family_map.size();
		auto limit = (factorial(size) / size) * (size - 1);

		auto max_happiness = PairHappiness{};

		do {
			// since the seating is circular, the pair [first, last] won't be
			// added in the algorithm, so we initialize the accumulator with it
			const auto begin1 = seating.begin();
			const auto end1	  = std::prev(seating.end());
			const auto begin2 = std::next(begin1);
			const auto acc	  = happymeter[{seating.front(), seating.back()}];
			const auto op1	  = std::plus{};
			const auto op2	  = [&happymeter] (auto a, auto b) { return happymeter[{a, b}];};

			// inner_product resolves to acc = op1(acc, op2(begin1, begin2)) in a loop
			auto sum = std::inner_product(begin1, end1, begin2, acc, op1, op2);

			max_happiness = std::max(max_happiness, sum);

		} while((limit--) > 0 && std::next_permutation(seating.begin(), seating.end()));

		std::cout << max_happiness << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
