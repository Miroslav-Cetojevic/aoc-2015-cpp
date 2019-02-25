#include <algorithm>
#include <iostream>
#include <vector>

struct Entity {
	std::size_t health;
	std::size_t damage;
	std::size_t armor;
};

struct Item {
	std::size_t price;
	std::size_t stat;
};

int main() {

	auto weapons = std::vector<Item>{{8,4}, {10,5}, {25,6}, {40,7}, {74,8}};
	auto armors = std::vector<Item>{{}, {13,1}, {31,2}, {53,3}, {75,4}, {102,5}};
	auto rings_dmg = std::vector<Item>{{}, {25,1}, {50,2}, {100,3}};
	auto rings_ac = std::vector<Item>{{}, {20,1}, {40,2}, {80,3}};

	auto max_gold = 0UL;

	auto boss = Entity{103, 9, 2};
	Entity hero; // no need to initialize, the innermost loop provides the values

	// the four loops is basically an implementation of
	// the cartesian product of the four vectors above
	for(auto& weapon : weapons) {

		for(auto& armor : armors) {

			for(auto& ring_dmg : rings_dmg) {

				for(auto& ring_ac : rings_ac) {

					hero = {100, (weapon.stat + ring_dmg.stat), (armor.stat + ring_ac.stat)};

					auto hero_real_dmg = (hero.damage - boss.armor);
					auto boss_real_dmg = (boss.damage - hero.armor);

					auto hero_rounds = ((hero.health / boss_real_dmg) + ((hero.health % boss_real_dmg) > 0));
					auto boss_rounds = ((boss.health / hero_real_dmg) + ((boss.health % hero_real_dmg) > 0));

					if(hero_rounds < boss_rounds) {
						max_gold = std::max(max_gold, (weapon.price + armor.price + ring_dmg.price + ring_ac.price));
					}
				}
			}
		}
	}

	std::cout << max_gold << std::endl;

	return 0;
}
