#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/range/counting_range.hpp>

struct Entity {
	std::int64_t health;
	std::int64_t damage;
	std::int64_t armor;
};

struct Item {
	std::int64_t price;
	std::int64_t dmg_bonus;
	std::int64_t ac_bonus;
};

template<typename T>
auto factorial(T n) -> T {
    if(n > 1) {
        return n * factorial(n - 1);
    }
    return 1;
}

template<typename N, typename K>
auto binomial_coefficient(N n, K k) {
	return factorial(n) / (factorial(k) * factorial(n-k));
}

// NOTE: this function is adapted from https://stackoverflow.com/a/9430993/699211
// It works by creating a "selection array", where we place k selectors, then
// we create all permutations of these selectors, and add the corresponding set
// member if it is selected in in the current permutation of the selectors.
auto get_rings() {

	auto rings = std::vector<Item>{{}, {}, {25,1,0}, {50,2,0}, {100,3,0}, {20,0,1}, {40,0,2}, {80,0,3}};

	// a combination of n rings with k elements in each combination
	const auto n = rings.size();
	const auto k = 2;

	// the binomial coefficient gives us the number of all combinations of size k from n elements
	auto ring_combos = std::vector<std::vector<Item>>(binomial_coefficient(n, k));

	enum class Bool { False, True };

	auto selector = std::vector<Bool>(n);

	std::fill(selector.begin(), selector.begin() + k, Bool::True);

	do {

		for(const auto i : boost::counting_range({}, n)) {

			if(selector[i] == Bool::True) {

				// the vectors containing the ring combinations must not exceed k elements
				auto fillable = std::find_if(ring_combos.begin(), ring_combos.end(), [k] (const auto combo) {
					return combo.size() < k;
				});

				// ring_combos is sized to contain all the combinations of length k,
				// so we are guaranteed that fillable never points to end()
				fillable->push_back(rings[i]);
			}
		}

	} while(std::prev_permutation(selector.begin(), selector.end()));

	return ring_combos;
}

int main() {

	auto weapons = std::vector<Item>{{8,4,0}, {10,5,0}, {25,6,0}, {40,7,0}, {74,8,0}};
	auto armors = std::vector<Item>{{}, {13,0,1}, {31,0,2}, {53,0,3}, {75,0,4}, {102,0,5}};
	auto rings = get_rings();

	auto min_gold = std::numeric_limits<decltype(Item::price)>::max();

	const auto boss = Entity{103, 9, 2};
	Entity hero; // no need to initialize, the innermost loop provides the values

	// cartesian product of all the equipment vectors
	for(const auto& weapon : weapons) {

		for(const auto& armor : armors) {

			for(const auto& ring : rings) {

				const auto& ring1 = ring.front();
				const auto& ring2 = ring.back();

				hero = {
					100,
					(weapon.dmg_bonus + ring1.dmg_bonus + ring2.dmg_bonus),
					(armor.ac_bonus + ring1.ac_bonus + ring2.ac_bonus)
				};

				const auto hero_dmg = (hero.damage - boss.armor);
				const auto boss_dmg = (boss.damage - hero.armor);

				const auto hero_real_dmg = (hero_dmg > 0) ? hero_dmg : 1;
				const auto boss_real_dmg = (boss_dmg > 0) ? boss_dmg : 1;

				const auto hero_health = hero.health;
				const auto boss_health = boss.health;

				// fight sequence is always the same, so we can just calculate how many rounds they both last
				const auto hero_rounds = ((hero_health / boss_real_dmg) + ((hero_health % boss_real_dmg) > 0));
				const auto boss_rounds = ((boss_health / hero_real_dmg) + ((boss_health % hero_real_dmg) > 0));

				// since the hero always attacks first in any given round,
				// equal rounds means the hero always wins
				if(hero_rounds >= boss_rounds) {
					min_gold = std::min(min_gold, (weapon.price + armor.price + ring1.price + ring2.price));
				}
			}
		}
	}

	std::cout << min_gold << std::endl;

	return 0;
}
