#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// std::include is used later in the code, which requires a sorted container,
// and I found std::map to be a very convenient tool for it
using Clues = std::map<std::string, unsigned>;

struct AuntSue {
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

auto extract_items_of_interest(Clues& sue, const Clues& differences) {

	auto items_of_interest = Clues{};

	for(const auto& difference : differences) {
		items_of_interest.insert(sue.extract(difference.first));
	}

	return items_of_interest;
}

auto find_real_sue(const std::vector<AuntSue>& aunties) {

	static const auto CHILDREN = std::string{"children"};
	static const auto CATS = std::string{"cats"};
	static const auto SAMOYEDS = std::string{"samoyeds"};
	static const auto POMERANIANS = std::string{"pomeranians"};
	static const auto AKITAS = std::string{"akitas"};
	static const auto VIZSLAS = std::string{"vizslas"};
	static const auto GOLDFISH = std::string{"goldfish"};
	static const auto TREES = std::string{"trees"};
	static const auto CARS = std::string{"cars"};
	static const auto PERFUMES = std::string{"perfumes"};

	static const auto sue = Clues{
		{CHILDREN, 3},
		{SAMOYEDS, 2},
		{AKITAS, 0},
		{VIZSLAS, 0},
		{CARS, 2},
		{PERFUMES, 1}
	};

	static const auto differences = Clues{
		{CATS, 7},
		{POMERANIANS, 3},
		{GOLDFISH, 5},
		{TREES, 3},
	};

	// need std::function to make use of std::greater and std::less
	using cmp = std::function<bool(unsigned, unsigned)>;

	static auto cmp_map = std::unordered_map<std::string, cmp>{
		{CATS, std::greater{}},
		{TREES, std::greater{}},
		{POMERANIANS, std::less{}},
		{GOLDFISH, std::less{}}
	};

	auto real_sue = std::find_if(aunties.begin(), aunties.end(), [] (auto& aunt) {

		auto other_sue = aunt.clues;

		// in order to compare "sue" with "other sue", first we need to extract
		// the items we know "sue" has, but their amounts are uncertain
		auto items_of_interest = extract_items_of_interest(other_sue, differences);

		auto is_subset = std::includes(sue.begin(), sue.end(), other_sue.begin(), other_sue.end());

		const auto begin = items_of_interest.begin();
		const auto end   = items_of_interest.end();
		const auto init  = is_subset;
		const auto op    = [] (auto acc, auto& item_of_interest) {

			const auto item = differences.find(item_of_interest.first);

			const auto found_item = (item != differences.end());

			const auto key = item->first;
			const auto comparator = item->second;
			const auto amount = item_of_interest.second;

			const auto result = (found_item ? cmp_map[key](amount, comparator) : true);

			return (acc && result);
		};

		// is_found can only be true if all items of interest have the differences we are looking for
		auto is_found = std::accumulate(begin, end, init, op);

		return is_found;
	});

	// if the input file wasn't guaranteed to have the result we are looking for,
	// a comparison with the end-iterator would have been necessary before accessing
	// the object, but since this is AoC, we do have the (implicit) guarantee
	return real_sue->id;
}

int main() {

	const auto filename = std::string{"aunts.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto aunties = std::vector<AuntSue>{};

		auto ss = std::stringstream{};

		std::string line;

		while(std::getline(file, line)) {

			ss.str(line);

			auto aunt = AuntSue{};

			ss >> aunt.id;

			Clue clue;

			while(ss >> clue) {
				aunt.clues[clue.name] = clue.amount;
			}

			aunties.push_back(aunt);

			ss.clear();
		}

		std::cout << find_real_sue(aunties) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
