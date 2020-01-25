#include <algorithm>
#include <array>
#include <iostream>
#include <limits>
#include <map>
#include <vector>

#include <boost/range/irange.hpp>

/* ====================
 * SHOP INVENTORY BEGIN
 * ====================
 */
// the dmg and ac values can be represented by the index, since they are sequential
constexpr auto weapons = std::array{ 8, 10, 25, 40, 74 };
constexpr auto armors  = std::array{ 0, 13, 31, 53, 75, 102 }; // zero cost for no armor

constexpr auto rings_dmg = std::array{ 25, 50, 100 };
constexpr auto rings_ac  = std::array{ 20, 40, 80 };

/* ====================
 * SHOP INVENTORY END
 * ====================
 */

/* =====================
 * PRECALCULATIONS BEGIN
 * =====================
 */
constexpr auto MAX_NUM_RINGS = 2;
constexpr auto MIN_WPN_DMG   = 4;
constexpr auto MIN_DMG       = 1;

template<typename T>
constexpr auto factorial(T n) -> T {
    return (n > 1) ? (n * factorial(n-1)) : 1;
}

template<typename N, typename K>
constexpr auto binomial_coefficient(N n, K k) {
  return (factorial(n) / (factorial(k) * factorial(n-k)));
}

constexpr auto num_ring_combos(const int n) {
  auto num_combos = 0;
  for(auto k = 0; k <= MAX_NUM_RINGS; ++k) {
    num_combos += binomial_coefficient(n, k);
  }
  return num_combos;
}

struct RingComboStats {
  int num_rings,
      rings_cost,
      bonuses;
};

template<typename Array>
constexpr auto ring_combo_stats(Array rings) {

  constexpr auto rings_size = rings.size();
  constexpr auto array_size = num_ring_combos(rings_size);

  auto stats = std::array<RingComboStats, array_size>{};

  // first combo is the one with no rings
  stats.front() = {};

  auto current = 1;

  for(auto i = 0; i < static_cast<decltype(i)>(rings_size); ++i) {

    const auto ring_i = rings[i];

    // the i'th ring (0,1,2) has a bonus of (1,2,3) -> (i + 1)
    stats[current++] = { 1 , ring_i, (i + 1) };

    for(auto j = (i + 1); j < static_cast<decltype(j)>(rings_size); ++j) {

      // the i'th + j'th rings have a bonus of (i+1) + (j+1) -> (i + j + 2)
      stats[current++] = { 2, (ring_i + rings[j]), (i + j + 2) };
    }
  }

  return stats;
}

constexpr auto ring_dmg_combos = ring_combo_stats(rings_dmg);
constexpr auto ring_ac_combos  = ring_combo_stats(rings_ac);

template<typename T, typename U>
constexpr auto costs_size(const T& items, const U& ring_combos) {
  const auto max_combo_cost = std::max_element(ring_combos.begin(), ring_combos.end(), [] (const auto& lhs, const auto& rhs) {
    return lhs.bonuses < rhs.bonuses;
  });
  return (items.size() + max_combo_cost->bonuses);
}

constexpr auto dmg_costs_size = costs_size(weapons, ring_dmg_combos);
constexpr auto ac_costs_size  = costs_size(armors, ring_ac_combos);

struct ComboCost {
  int total_cost,
      num_rings;
};

struct CostLookup {
  std::array<std::vector<ComboCost>, dmg_costs_size> dmg_combo_costs;
  std::array<std::vector<ComboCost>, ac_costs_size>  ac_combo_costs;
};

auto precalc_combo_costs() {

  auto lookup = CostLookup{};

  const auto precalc = [] (const auto& items, const auto& ring_combos, auto& lookup) {
    for(const auto i : boost::irange(items.size())) {
      const auto item_cost = items[i];
      for(const auto& combo_stats : ring_combos) {
        lookup[i+combo_stats.bonuses].push_back({
          (item_cost + combo_stats.rings_cost),
          combo_stats.num_rings
        });
      }
    }
  };

  precalc(weapons, ring_dmg_combos, lookup.dmg_combo_costs);
  precalc(armors, ring_ac_combos, lookup.ac_combo_costs);

  return lookup;
}

/* =====================
 * PRECALCULATIONS END
 * =====================
 */

struct Entity {
  int health,
      damage,
      armor;
};

auto run_game(const CostLookup& lookup, const Entity& boss, const int player_hp) {

  const auto min_armor_to_win = [&] (const int player_dmg) {
    const auto player_real_dmg = std::max((player_dmg - boss.armor), MIN_DMG);
    const auto num_rounds_to_win = ((boss.health / player_real_dmg) + ((boss.health % player_real_dmg) > 0));
    const auto num_rounds_to_last = (num_rounds_to_win - 1);
    const auto max_health_loss_per_round = (player_hp / num_rounds_to_last);
    // if player has no hp left after num_rounds_to_last, then they need extra armor
    const auto extra_armor = ((player_hp % num_rounds_to_last) == 0);

    return (boss.damage - max_health_loss_per_round + extra_armor);
  };

  auto min_cost = std::numeric_limits<int>::max();

  const auto& dmg_combo_costs = lookup.dmg_combo_costs;
  const auto& ac_combo_costs  = lookup.ac_combo_costs;

  for(const auto i : boost::irange(dmg_combo_costs.size())) {
    /*
     * MIN_WPN_DMG is there because we need to adjust the damage value
     * represented by the index of the array we are iterating over,
     * which was started from 0 for convenience in the precalculations
     */
    const auto player_dmg = (i + MIN_WPN_DMG);

    const auto min_armor = min_armor_to_win(player_dmg);

    using Type = std::decay_t<decltype(min_armor)>;

    // suppress gcc warning
    if(min_armor < static_cast<Type>(ac_combo_costs.size())) {

      for(const auto& ac_combo_cost : ac_combo_costs[min_armor]) {

        for(const auto& dmg_combo_cost : dmg_combo_costs[i]) {

          const auto num_rings = (ac_combo_cost.num_rings + dmg_combo_cost.num_rings);

          if(num_rings <= MAX_NUM_RINGS) {

            const auto new_cost = (ac_combo_cost.total_cost + dmg_combo_cost.total_cost);

            min_cost = std::min(new_cost, min_cost);
          }
        }
      }
    }
  }

  return min_cost;
}

auto solution(const Entity& boss, const int player_hp) {
  const auto& lookup = precalc_combo_costs();
  const auto min_cost = run_game(lookup, boss, player_hp);
  return min_cost;
}


int main() {

  const auto boss = Entity{103, 9, 2};
  const auto player_hp = 100;

  std::cout << solution(boss, player_hp) << std::endl;

}
