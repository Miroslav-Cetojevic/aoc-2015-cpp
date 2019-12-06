#include <algorithm>
#include <iostream>
#include <vector>

// ========
// Entities
// ========

struct Hero {
	std::int64_t health;
	std::uint64_t mana;
	std::uint64_t armor;
};

struct Boss {
	std::int64_t health;
	std::uint64_t damage;
};

// ================
// Spells & Effects
// ================

// need to forward declare these structs
// so I can use them for the function pointer
struct GameState;
struct Page;
using DoMagic = void (*) (GameState&, Page&);

struct Effect {
	DoMagic apply;
	std::int64_t value;
	std::uint64_t turns_max;
	std::uint64_t turns_left;
};

struct Spell {
	DoMagic cast;
	std::uint64_t cost;
	std::uint64_t value;
};

struct Page {
	Effect effect;
	Spell spell;
	std::uint64_t id;
};

using Spellbook = std::vector<Page>;
struct GameState {
	Hero hero;
	Boss boss;
	Spellbook spellbook;
	std::uint64_t current_page;
	std::uint64_t mana_spent;
};

auto subtract_mana(GameState& state, const Page& page) {
	state.hero.mana -= page.spell.cost;
}

auto subtract_boss_health(GameState& state, const Page& page) {
	state.boss.health -= page.spell.value;
}

auto calc_mana_spent(GameState& state, const Page& page) {
	state.mana_spent += page.spell.cost;
}

auto init_effect(GameState& state, Page& page) {
	subtract_mana(state, page);
	page.effect.turns_left = page.effect.turns_max;
}

auto apply_immediate_effects(GameState& state, const Page& page) {
	subtract_mana(state, page);
	subtract_boss_health(state, page);
	calc_mana_spent(state, page);
}

auto cast_missile(GameState& state, Page& page) {
	apply_immediate_effects(state, page);
}

auto cast_drain(GameState& state, Page& page) {
	apply_immediate_effects(state, page);
	state.hero.health += page.spell.value;
}

auto activate_effect(GameState& state, Page& page) {
	init_effect(state, page);
	calc_mana_spent(state, page);
}

auto cast_poison(GameState& state, Page& page) {
	activate_effect(state, page);
}

auto cast_recharge(GameState& state, Page& page) {
	activate_effect(state, page);
}

auto cast_shield(GameState& state, Page& page) {
	activate_effect(state, page);
	state.hero.armor = page.spell.value;
}

auto apply_missile(GameState&, Page&) {}
auto apply_drain(GameState&, Page&) {}

template<typename F>
auto apply_effect(GameState& state, Page& page, F effect) {
	if(page.effect.turns_left > 0) {
		--(page.effect.turns_left);
		effect(state, page);
	}
}

auto apply_poison(GameState& state, Page& page) {
	apply_effect(state, page, [] (auto& state, auto& page) {
		state.boss.health -= page.effect.value;
	});
}

auto apply_recharge(GameState& state, Page& page) {
	apply_effect(state, page, [] (auto& state, auto& page) {
		state.hero.mana += page.effect.value;
	});
}

auto apply_shield(GameState& state, Page& page) {
	apply_effect(state, page, [] (auto& state, auto& page) {
		if(page.effect.turns_left == 0) {
			state.hero.armor = 0;
		}
	});
}

auto boss_attack(GameState& state) {
	const auto boss_dmg = state.boss.damage;
	const auto hero_ac = state.hero.armor;

	const auto boss_real_dmg = (hero_ac < boss_dmg) ? (boss_dmg - hero_ac) : 1;

	state.hero.health -= boss_real_dmg;
}

// ========
// The Game
// ========

int main() {

	const auto spellbook = Spellbook{
		{{apply_missile, 0, 0, 0}, {cast_missile, 53, 4}, 0},
		{{apply_drain, 0, 0, 0}, {cast_drain, 73, 2}, 1},
		{{apply_shield, 0, 6, 0}, {cast_shield, 113, 7}, 2},
		{{apply_poison, 3, 6, 0}, {cast_poison, 173, 0}, 3},
		{{apply_recharge, 101, 5, 0}, {cast_recharge, 229, 0}, 4}
	};

	auto hero = Hero{50, 500, 0};
	auto boss = Boss{55, 8};

	auto states = std::vector<GameState>{{hero, boss, spellbook, 0, 0}};
	auto least_mana = std::numeric_limits<decltype(hero.mana)>::max();

	const auto save_state = [&states] (auto& state) {
		// each new state means re-reading the spellbook from the beginning
		state.current_page = 0;
		states.push_back(state);
	};

	const auto delete_state = [&states] () {
		states.pop_back();
	};

	const auto apply_effects = [] (auto& state) {
		for(auto& page : state.spellbook) {
			page.effect.apply(state, page);
		}
	};

	const auto has_won = [] (const auto& state) {
		return (state.boss.health <= 0);
	};

	const auto has_lost = [] (const auto& state) {
		return (state.hero.health <= 0);
	};

	const auto end_of_spellbook = [] (const auto& state) {
		return (state.current_page == state.spellbook.size());
	};

	const auto get_least_mana = [least_mana, &delete_state] (const auto& state) {
		// a victory has been attained, the last state is not needed anymore
		delete_state();
		// the state argument will be a reference to the copy of the state
		// that we just deleted, so it's safe to refer to it
		return std::min(least_mana, state.mana_spent);
	};

	// each iteration (and thus each state) represents two turns played
	do {
		auto state = GameState(states.back());

		// ===========
		// hero's turn
		// ===========

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

		const auto begin = state.spellbook.begin() + state.current_page;
		const auto end   = state.spellbook.end();

		const auto check = [&state, least_mana] (auto& page) {
			const auto mana_spent = state.mana_spent;
			const auto spell_cost = page.spell.cost;
			const auto turns_left = page.effect.turns_left;
			const auto hero_mana  = state.hero.mana;

			const auto reduces_mana_cost = (least_mana > 0) && ((mana_spent + spell_cost) >= least_mana);
			const auto is_spell_castable = (turns_left == 0) && (hero_mana >= spell_cost);

			return !reduces_mana_cost && is_spell_castable;
		};

		const auto eligible_spell = std::find_if(begin, end, check);

		const auto has_castable_spell = (eligible_spell != state.spellbook.end());

		if(has_castable_spell) {
			// the hero attacks
			eligible_spell->spell.cast(state, *eligible_spell);
			// if we return to this state later on, the hero may only cast the next spell,
			// because casting any of the previous ones will simply result in an already visited scenario
			++(states.back().current_page);
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

		// ==========
		// boss' turn
		// ==========

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
