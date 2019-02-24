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

#include <chrono>
auto now = [] { return std::chrono::steady_clock::now(); };

struct AuntSue {
	std::size_t id;
	std::map<std::size_t, std::size_t> detectables;
};

struct Detectable {
	std::size_t amount;
	std::string name;
};

auto& operator>>(std::istream& in, Detectable& detectable) {
	return in >> detectable.name >> detectable.amount;
}

template<typename T>
auto get_hash(T t) { return std::hash<T>{}(t); }

struct Items {
	std::size_t children = get_hash(std::string{"children"});
	std::size_t cats = get_hash(std::string{"cats"});
	std::size_t samoyeds = get_hash(std::string{"samoyeds"});
	std::size_t pomeranians = get_hash(std::string{"pomeranians"});
	std::size_t akitas = get_hash(std::string{"akitas"});
	std::size_t vizslas = get_hash(std::string{"vizslas"});
	std::size_t goldfish = get_hash(std::string{"goldfish"});
	std::size_t trees = get_hash(std::string{"trees"});
	std::size_t cars = get_hash(std::string{"cars"});
	std::size_t perfumes = get_hash(std::string{"perfumes"});
};

template<typename D>
auto extract_items_of_interest(const D& differences, D& detectables) {
	auto items_of_interest = D{};

	for(const auto& difference : differences) {
		items_of_interest.insert(detectables.extract(difference.first));
	}

	return items_of_interest;
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"aunts.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto start = now();

		auto aunties = std::vector<AuntSue>{};

		auto ss = std::stringstream{};

		std::string line;
		while(std::getline(file, line)) {

			ss.str(line);

			auto aunt = AuntSue{};

			ss >> aunt.id;

			Detectable detectable;
			while(ss >> detectable) {
				aunt.detectables[get_hash(detectable.name)] = detectable.amount;
			}

			ss.clear();

			aunties.emplace_back(aunt);
		}

		auto items = Items{};

		decltype(AuntSue::detectables) sue{
			{items.children, 3},
			{items.samoyeds, 2},
			{items.akitas, 0},
			{items.vizslas, 0},
			{items.cars, 2},
			{items.perfumes, 1}
		};

		decltype(AuntSue::detectables) differences{
			{items.cats, 7},
			{items.pomeranians, 3},
			{items.goldfish, 5},
			{items.trees, 3},
		};

		using cmp = std::function<bool(const std::size_t, const std::size_t)>;
		auto cmp_map = std::unordered_map<std::size_t, cmp>{
			{items.cats, std::greater{}},
			{items.trees, std::greater{}},
			{items.pomeranians, std::less{}},
			{items.goldfish, std::less{}}
		};

		auto real_sue = std::find_if(aunties.begin(), aunties.end(), [&sue, &differences, &cmp_map] (auto& aunt) {
			auto other_sue = aunt.detectables;

			auto items_of_interest = extract_items_of_interest(differences, other_sue);

			auto is_subset = std::includes(sue.begin(), sue.end(), other_sue.begin(), other_sue.end());

			auto is_found = std::accumulate(items_of_interest.begin(), items_of_interest.end(), is_subset,
											[&differences, &cmp_map] (auto acc, auto& item_of_interest) {
				auto item = differences.find(item_of_interest.first);
				auto found_item = (item != differences.end());
				return (acc && (found_item ? cmp_map[item->first](item_of_interest.second, item->second) : true));
			});

			return is_found;
		});

		std::cout << real_sue->id << std::endl;

		auto finish = now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish-start).count();
		std::cout << duration << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
