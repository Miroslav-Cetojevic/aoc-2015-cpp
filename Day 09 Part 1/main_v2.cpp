#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
#include <iostream>
#include <queue>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "input.hpp"

using uintmax = std::uintmax_t;

using Location = std::string_view;
using LocationID = uintmax;

using Visits = std::bitset<8>;

struct Path {
	LocationID target;
	uintmax distance;
};

auto split_line(std::string_view line) {

  /*
   * The number of elements is the same on each line,
   * so we use array instead of vector
   */
	auto tokens = std::array<std::string_view, 5>{};

	decltype(line.size()) pos = {};
	auto index = pos;

	while (pos != line.npos) {
		pos = line.find(' ');
		tokens[index++] = line.substr({}, pos);
		line.remove_prefix(pos + 1);
	}

	return tokens;
}

auto split_input(std::string_view input) {

	auto lines = std::vector<std::string_view>{};

	decltype(input.size()) pos = {};

	while (pos != input.npos) {
		pos = input.find('\n');
		lines.push_back(input.substr({}, pos));
		input.remove_prefix(pos + 1);
	}

	return lines;
}

auto parse_input(std::string_view input) {

	auto paths = std::vector<std::vector<Path>>{};

	using Locations = std::unordered_map<Location, LocationID>;

	auto find_or_insert = [&paths, locations = Locations{}, id = LocationID{}] (const auto location) mutable {

		const auto not_mapped = (locations.find(location) == locations.end());

		if(not_mapped) {
			locations[location] = id++;
			paths.push_back({});
		}

		return locations.at(location);
	};

	const auto to_int = [] (std::string_view s) {
		uintmax value;
		std::from_chars(s.begin(), s.end(), value);
		return value;
	};

	for(auto line : split_input(input)) {

		const auto tokens = split_line(line);

		const auto location_a = find_or_insert(tokens[0]);
		const auto location_b = find_or_insert(tokens[2]);

		const auto distance = to_int(tokens[4]);

		paths[location_a].push_back({location_b, distance});
		paths[location_b].push_back({location_a, distance});
	}

	return paths;
}

struct State {
	uintmax first, last;
	Visits visits;

	/*
	 * Always make sure the locations (first and last) are sorted by id so we can
	 * avoid creating equivalent states, thus reducing the search space in half.
	 *
	 * E.g. A->B->C is equivalent to C->B->A.
	 */
	State(uintmax f, uintmax l, Visits v):
		first(std::min(f, l)),
		last(std::max(f, l)),
		visits(v) {};

	// this will be called by the comparator of the priority queue in the A* search algorithm
	auto operator<(State other) const {
		const auto lhs = visits.to_ulong();
		const auto rhs = other.visits.to_ulong();
		return std::tie(first, last, lhs) < std::tie(other.first, other.last, rhs);
	}

	// this is required so we can use State as a key in an unordered map
	auto operator==(State other) const {
	  return first == other.first
	         and last == other.last
	         and visits == other.visits;
	}
};

namespace std {
  template<>
  struct hash<State> {
    auto operator()(const State& state) const {
      return state.first
             + (state.last << 8)
             + (state.visits.to_ulong() << 16);
    }
  };
}

/*
 * An implementation of the A* search algorithm
 *
 * Score represents the total distance between points A and B.
 */
template<typename Score, typename State, typename Heuristic, typename Goal, typename Next>
auto a_star(State start, Heuristic&& heuristic, Goal&& is_goal, Next&& for_each_neighbor) {

  auto result = std::pair{start, Score{}};

	// We need reverse ordering, so the smallest elements are accessed first
	const auto cmp = [](auto lhs, auto rhs) {return lhs.first > rhs.first;};

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

		  if(not is_goal(state)) {

		    const auto add_next_state = [&] (State next, Score distance) {

		      const auto new_score = (g_score + distance);

		      const auto next_not_found = [&scores, next] {
		          return (scores.find(next) == scores.end());
		      };

		      const auto new_score_is_better = [&scores, next, new_score] {
		        return new_score < scores[next];
		      };

		      if(next_not_found() or new_score_is_better()) {
		        scores[next] = new_score;
		        queue.emplace((new_score + heuristic(next)), next);
		      }
		    };

		    // We advance to a next state by adding a new place to either end of the current state
		    for_each_neighbor(state, add_next_state);

		  } else {
		    result = {state, g_score};
		    break;
		  }
		}
	}

	return result;
}

auto solution(std::string_view input) {

	const auto paths = parse_input(input);

	/*
	 * A good heuristic will never overestimate the cost the next step should take
	 * and reduces the search space.
	 *
	 * Normally, it would perform the lookup part of for_each_neighbor() and find the
	 * minimum value of the available costs (distances, in this case).
	 *
	 * For this particular problem, the search space is small enough that a heuristic
	 * of zero makes the search as fast as possible. The cost of any lookup operation
	 * will outweigh any reduction of search space, therefore making the search slower.
	 *
	 * Note: A* with a heuristic of zero is equivalent to Dijkstra's search.
	 */
  auto heuristic = [] (State) {
    return 0;
  };

	const auto is_goal = [&paths] (State state) {
		return (state.visits.count() == paths.size());
	};

	/*
	 * Each state has a point of origin (first) and a destination (last). Both locations have
	 * a "chain" of sub-paths where they act as the point of origin, and the sub-paths show
	 * the distances to other destinations. A neighbor of a state is another state, and its
	 * locations (first and last) are the destinations from the aforementioned sub-paths that
	 * have not been visited yet.
	 *
	 * Every time for_each_neighbor is called, we check which of the sub-paths for which the
	 * locations of the passed state act as the point of origin have not been traveled yet. The
	 * destinations of those sub-paths will be part of the new state we construct (the neighbor),
	 * which will be used to repeat the process, but with different locations, until no subpaths
	 * remain.
	 */
  const auto for_each_neighbor = [&paths] (State state, auto&& callback) {

      const auto iterate = [&] (auto a, auto b) {

        const auto visits = state.visits;

        for(auto subpath : paths[a]) {

          const auto target = subpath.target;
          const auto target_not_visited = !visits.test(target);

          if(target_not_visited) {
            auto next = State{target, b, visits};
            next.visits.set(target);

            /*
             * A* will supply the callback function that will
             * add the next possible state (neighbor) to explore
             * to the queue
             */
            callback(next, subpath.distance);
          }
        }
      };

      const auto first = state.first;
      const auto last = state.last;

      iterate(first, last);
      iterate(last, first);
  };

	const auto begin_state = State{0, 0, 1};

	return a_star<uintmax>(begin_state, heuristic, is_goal, for_each_neighbor).second;
}

int main() {

  std::cout << solution(puzzle_input) << std::endl;

}
