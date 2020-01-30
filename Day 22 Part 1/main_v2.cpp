#include <array>
#include <iostream>
#include <queue>
#include <unordered_map>

/*
 * ===============
 * DATA STRUCTURES
 * ===============
 */
//These are all stats that differ between states
struct GameState {
  int player_hp,
      player_mana,
      boss_hp,
      missile_turns  = 0,
      drain_turns    = 0,
      shield_turns   = 0,
      poison_turns   = 0,
      recharge_turns = 0;
};

auto operator==(const GameState& lhs, const GameState& rhs) {
  return std::equal_to{}(
    std::tie(lhs.player_hp, lhs.player_mana, lhs.boss_hp, lhs.shield_turns, lhs.poison_turns, lhs.recharge_turns),
    std::tie(rhs.player_hp, rhs.player_mana, rhs.boss_hp, rhs.shield_turns, rhs.poison_turns, rhs.recharge_turns)
  );
}

constexpr auto left_shift(std::size_t value, std::size_t bits = 0) {
  return value << bits;
}

// hash specialization for using GameState as a key in an unordered map
namespace std {
  template<>
  struct hash<GameState> {
    auto operator()(const GameState& state) const {
      // missile_/drain_time are always 0, so we have 6 relevant fields to encode
      return left_shift(state.player_hp)
             | left_shift(state.player_mana, 10)
             | left_shift(state.boss_hp, 20)
             | left_shift(state.shield_turns, 30)
             | left_shift(state.recharge_turns, 40)
             | left_shift(state.poison_turns, 50);
    }
  };
}

struct Spell {
  int mana_cost,
      player_hp_diff,
      boss_hp_diff,
      max_turns,
      GameState::* turns;
  auto cast(GameState& state) const {
    const auto castable = ((mana_cost <= state.player_mana) and (state.*turns <= 0));
    if(castable) {
      state.player_mana -= mana_cost;
      state.player_hp   += player_hp_diff;
      state.boss_hp     -= boss_hp_diff;
      state.*turns       = max_turns;
    }
    return castable;
  }
};

constexpr auto missile  = Spell{  53, 0, 4, 0, &GameState::missile_turns };
constexpr auto drain    = Spell{  73, 2, 2, 0, &GameState::drain_turns };
constexpr auto shield   = Spell{ 113, 0, 0, 6, &GameState::shield_turns };
constexpr auto poison   = Spell{ 173, 0, 0, 6, &GameState::poison_turns};
constexpr auto recharge = Spell{ 229, 0, 0, 5, &GameState::recharge_turns};

template<typename T, std::size_t N, std::size_t... I>
constexpr auto to_array(T (&&array)[N], std::index_sequence<I...>) -> std::array<T, N> {
  return {std::move(array[I])...};
}

template<typename T, std::size_t N>
constexpr auto make_array(T (&&array)[N]) {
  return to_array(std::move(array), std::make_index_sequence<N>{});
}

constexpr auto spells = make_array<Spell>({
  missile, drain, shield, poison, recharge
});

// effects not handled by the spells
constexpr auto SHIELD_DEF    = 7;
constexpr auto POISON_DMG    = 3;
constexpr auto RECHARGE_MANA = 101;

constexpr auto NUM_MISSILES = 2;
constexpr auto MAX_DMG_COMBO = ((NUM_MISSILES * missile.boss_hp_diff) + (poison.max_turns * POISON_DMG));
constexpr auto MAX_DMG_COMBO_MANA = static_cast<double>((NUM_MISSILES * missile.mana_cost) + poison.mana_cost);
constexpr auto AVG_MANA_PER_DMG = (MAX_DMG_COMBO_MANA / MAX_DMG_COMBO);
constexpr auto AVG_MANA_PER_DMG_HEURISTIC = (static_cast<int>(100 * AVG_MANA_PER_DMG) / 100.0);

struct PuzzleInput {
  int player_hp,
      player_mana,
      boss_hp,
      boss_dmg;
};

/*
 * =========
 * ALGORITHM
 * =========
 */
template<typename Score, typename State, typename Heuristic, typename Goal, typename Next>
auto a_star(const State& start, Heuristic&& heuristic, Goal&& goal, Next&& for_each_neighbor) {

  auto result = std::pair{start, Score{}};

  // We need reverse ordering, so the smallest elements are accessed first
  const auto cmp = [](const auto lhs, const auto rhs) {return lhs.first > rhs.first;};

  using T = std::pair<Score, State>;
  using Container = std::vector<T>;
  using Comparator = decltype(cmp);

  auto queue = std::priority_queue<T, Container, Comparator>{cmp};

  queue.emplace(heuristic(start), start);

  auto scores = std::unordered_map<State, Score>{
    {start, {}}
  };

  while(not queue.empty()) {

    const auto [f_score, state] = queue.top();

    queue.pop();

    // A* uses the notation f = g + h, where f is a function of the sum of g and h
    const auto g_score = scores[state];

    if(f_score - heuristic(state) <= g_score) {

      if(not goal(state)) {

        const auto add_next_state = [&] (State& next, Score mana_cost) {

          const auto new_score = (g_score + mana_cost);

          const auto next_state_is_new = [&] {
            return (scores.find(next) == scores.end());
          };

          const auto new_score_is_better = [&] {
            return new_score < scores[next];
          };

          if(next_state_is_new() or new_score_is_better()) {
            scores[next] = new_score;
            queue.emplace((new_score + heuristic(next)), next);
          }
        };

        // We advance to a next state
        for_each_neighbor(state, add_next_state);

      } else {
        result = {state, g_score};
        break;
      }
    }
  }

  return result;
}

auto solution(const PuzzleInput& input) {

  const auto start = GameState{
    input.player_hp,
    input.player_mana,
    input.boss_hp,
  };

  const auto heuristic = [] (const auto& state) {
    // this will determine if there's enough mana to pull off the most damaging spell combo
    return state.boss_hp * AVG_MANA_PER_DMG_HEURISTIC;
  };

  const auto goal = [] (const auto& state) {
    return state.boss_hp <= 0;
  };

  const auto apply_effects = [] (auto& state) {

    auto& shield_turns   = state.shield_turns;
    auto& poison_turns   = state.poison_turns;
    auto& recharge_turns = state.recharge_turns;

    if(shield_turns > 0) {
      --shield_turns;
    }

    if(poison_turns > 0) {
      state.boss_hp -= POISON_DMG;
      --poison_turns;
    }

    if(recharge_turns > 0) {
      state.player_mana += RECHARGE_MANA;
      --recharge_turns;
    }
  };

  const auto after_player = [&] (auto& state) {
    // player's turn after they have performed their spell
    apply_effects(state);
    // boss' turn
    if(state.boss_hp > 0) {
      state.player_hp -= (input.boss_dmg - (SHIELD_DEF * (state.shield_turns > 0)));
      if(state.player_hp > 0) {
        apply_effects(state);
      }
    }
  };

  const auto for_each_spell = [&] (const auto& state, auto&& callback) {
    for(const auto& spell : spells) {
      auto next_state = state;
      // this is where the turns are played out
      if(spell.cast(next_state)) {
        after_player(next_state);
        if(next_state.player_hp > 0) {
          callback(next_state, spell.mana_cost);
        }
      }
    }
  };

  return a_star<int>(start, heuristic, goal, for_each_spell).second;
}

int main() {
  const auto input = PuzzleInput{ 50, 500, 55, 8};
  std::cout << solution(input) << std::endl;
}
