#include <algorithm>
#include <iostream>
#include <vector>

// ========== Entities ========== //
struct Hero {
	std::ptrdiff_t health;
	std::size_t mana;
	std::size_t armor;
};

struct Boss {
	std::ptrdiff_t health;
	std::size_t damage;
};

// ========== Spells & Effects =========== //
struct GameState;
struct Page;
using DoMagic = void (*) (GameState&, Page&);

struct Effect {
	DoMagic apply;
	std::ptrdiff_t value;
	std::size_t turns_max;
	std::size_t turns_left;
};

struct Spell {
	DoMagic cast;
	std::size_t cost;
	std::size_t value;
};

struct Page {
	Effect effect;
	Spell spell;
	std::size_t id;
};

using Spellbook = std::vector<Page>;
struct GameState {
	Hero hero;
	Boss boss;
	Spellbook spellbook;
	std::size_t page_no;
	std::size_t mana_spent;
};

template<typename S, typename M>
auto subtract(S&& subtrahend, M&& minuend) {
	subtrahend -= minuend;
}

template<typename S, typename A>
auto add(S&& summand, A&& addend) {
	summand += addend;
}

auto calc_mana_spent(GameState& state, Page& page) {
	add(state.mana_spent, page.spell.cost);
}

auto init_effect(GameState& state, Page& page) {
	subtract(state.hero.mana, page.spell.cost);
	page.effect.turns_left = page.effect.turns_max;
}

auto cast_missile(GameState& state, Page& page) {
	subtract(state.hero.mana, page.spell.cost);
	subtract(state.boss.health, page.spell.value);
	calc_mana_spent(state, page);
}

auto cast_drain(GameState& state, Page& page) {
	subtract(state.hero.mana, page.spell.cost);
	subtract(state.boss.health, page.spell.value);
	add(state.hero.health, page.spell.value);
	calc_mana_spent(state, page);
}

auto cast_poison(GameState& state, Page& page) {
	init_effect(state, page);
	calc_mana_spent(state, page);
}

auto cast_recharge(GameState& state, Page& page) {
	init_effect(state, page);
	calc_mana_spent(state, page);
}

auto cast_shield(GameState& state, Page& page) {
	init_effect(state, page);
	state.hero.armor = page.spell.value;
	calc_mana_spent(state, page);
}

auto apply_missile(GameState&, Page&) {}
auto apply_drain(GameState&, Page&) {}

auto apply_poison(GameState& state, Page& page) {
	if(page.effect.turns_left > 0) {
		subtract(state.boss.health, page.effect.value);
		--(page.effect.turns_left);
	}
}

auto apply_recharge(GameState& state, Page& page) {
	if(page.effect.turns_left > 0) {
		add(state.hero.mana, page.effect.value);
		--(page.effect.turns_left);
	}
}

auto apply_shield(GameState& state, Page& page) {
	if(page.effect.turns_left > 0) {
		--(page.effect.turns_left);
		if(page.effect.turns_left == 0) {
			state.hero.armor = 0;
		}
	}
}

auto boss_attack(GameState& state) {
	auto boss_damage = (state.hero.armor < state.boss.damage) ? (state.boss.damage - state.hero.armor) : 1;
	subtract(state.hero.health, boss_damage);
}

int main() {

	auto spellbook = Spellbook{
		{{apply_missile, 0, 0, 0}, {cast_missile, 53, 4}, 0},
		{{apply_drain, 0, 0, 0}, {cast_drain, 73, 2}, 1},
		{{apply_shield, 0, 6, 0}, {cast_shield, 113, 7}, 2},
		{{apply_poison, 3, 6, 0}, {cast_poison, 173, 0}, 3},
		{{apply_recharge, 101, 5, 0}, {cast_recharge, 229, 0}, 4}
	};

	auto hero = Hero{50, 500, 0};
	auto boss = Boss{55, 8};

	auto states = std::vector<GameState>{{hero, boss, spellbook, 0, 0}};
	auto least_mana = 0UL;

	auto save_state = [&states] (auto& state) {
		// each new state means re-reading the spellbook from the beginning
		state.page_no = 0;
		states.push_back(state);
	};

	auto delete_state = [&states] { states.pop_back(); };

	auto apply_effects = [] (auto& state) {
		for(auto& page : state.spellbook) {
			page.effect.apply(state, page);
		}
	};

	auto has_won = [] (auto& state) { return (state.boss.health <= 0); };

	auto has_lost = [] (auto& state) { return (state.hero.health <= 0); };

	auto end_of_spellbook = [] (auto& state) { return (state.page_no == state.spellbook.size()); };

	auto get_least_mana = [least_mana, &delete_state] (auto& state) {
		// a victory has been attained, the last state is not needed anymore
		delete_state();
		return ((least_mana > 0) ? std::min(least_mana, state.mana_spent) : state.mana_spent);
	};



	// each iteration (and thus each state) represents two turns played
	do {
		auto state = GameState(states.back());

		/* hero's turn */
		--state.hero.health;
		if(has_lost(state)) {
			delete_state();
			continue;
		}

		apply_effects(state);

		if(has_won(state)) {
			least_mana = get_least_mana(state);
			continue;
		}

		// there's no spell to be cast, so we discard the current state
		if(end_of_spellbook(state)) {
			delete_state();
			continue;
		}

		auto eligible_spell = std::find_if((state.spellbook.begin() + state.page_no),
										   state.spellbook.end(),
										   [&state, least_mana] (auto& page) {
			auto better_solution_exists = (least_mana > 0) && ((state.mana_spent + page.spell.cost) >= least_mana);
			auto is_spell_castable = (page.effect.turns_left == 0) && (state.hero.mana >= page.spell.cost);

			return !better_solution_exists && is_spell_castable;
		});

		auto has_castable_spell = (eligible_spell != state.spellbook.end());

		if(has_castable_spell) {
			// the hero attacks
			eligible_spell->spell.cast(state, *eligible_spell);
			// if we return to this state later on, the hero may only cast the next spell,
			// because casting any of the previous ones will simply result in an already visited scenario
			++(states.back().page_no);
		}

		// no spells were cast either because
		// 1. there was not enough mana for any of them, or
		// 2. there's already a victory scenario with equal or less total mana spent
		if(!has_castable_spell) {
			delete_state();
			continue;
		}

		if(has_won(state)) {
			least_mana = get_least_mana(state);
			continue;
		}

		/* boss' turn */
		apply_effects(state);

		if(has_won(state)) {
			least_mana = get_least_mana(state);
			continue;
		}

		boss_attack(state);

		if(has_lost(state)) {
			delete_state();
			continue;
		}

		save_state(state);

	} while(!states.empty());

	std::cout << least_mana << std::endl;

	return 0;
}
