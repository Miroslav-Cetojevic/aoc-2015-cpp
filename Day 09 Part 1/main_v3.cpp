#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
#include <cmath>
#include <iostream>
#include <queue>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/range/irange.hpp>

#include "input.hpp"

using uintmax = std::uintmax_t;

using Location = std::string_view;
using LocationID = uintmax;
using Distance = uintmax;

constexpr auto NUM_LOCATIONS = 8;
using Visits = std::bitset<NUM_LOCATIONS>;

/*
 * Diagonal matrix
 *
 * (1,0)
 * (2,0), (2,1)
 * (3,0), (3,1), (3,2)
 */
struct AdjacencyMatrix {

  std::vector<Distance> matrix;
  uintmax num_locations;

  auto lookup(LocationID x, LocationID y) const {
    const auto triangle = [] (auto n) {
        return (n * (n - 1)) / 2;
    };
    const auto [min, max] = std::minmax(x, y);
    return matrix[triangle(max) + min];
  }
};

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

  auto paths = AdjacencyMatrix{};

  const auto to_int = [] (std::string_view s) {
    uintmax value;
    std::from_chars(s.begin(), s.end(), value);
    return value;
  };

  auto& matrix = paths.matrix;

  for(auto line : split_input(input)) {

      const auto last_token = line.find_last_of(' ') + 1;

      const auto distance = to_int(line.substr(last_token));

      matrix.push_back(distance);
  }

  /*
   * The matrix looks like this (conceptually):
   *
   * (1,0)
   * (2,0), (2,1)
   * (3,0), (3,1), (3,2)
   *
   * But the actual ordering is:
   *
   * (1,0), (2,0), (2,1), (3,0), (3,1), (3,2)
   *
   * However, in the input the distances are grouped by point of origin,
   * and the next group has alway one entry less to avoid the duplicate
   * entry from the previous group (remember: A->B is equal to B->A)
   *
   * So we end up with an ordering that would translate to:
   *
   * (3,2) (3,1) (3,0)
   * (2,1) (2,0)
   * (1,0)
   *
   * which won't work for an adjacency matrix. So we reverse the vector.
   */
  std::reverse(matrix.begin(), matrix.end());

  const auto triangle_root = [] (auto x) {
      return (static_cast<decltype(x)>(std::sqrt(8 * (x + 1))) - 1) / 2;
  };

  paths.num_locations = triangle_root(matrix.size()) + 1;

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
 * Score represents the total distance between the end points of any path.
 */
template<typename Score, typename State, typename Heuristic, typename Goal, typename Next>
auto a_star(State start, Heuristic&& heuristic, Goal&& is_goal, Next&& for_each_neighbor) {

  auto result = std::pair{start, Score{}};

  // We need reverse ordering, so the smallest elements can be accessed first
	const auto cmp = [](auto lhs, auto rhs) {
	  return lhs.first > rhs.first;
	};

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

		    const auto add_next = [&] (State next, Score distance) {

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
		    for_each_neighbor(state, add_next);

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
		return (state.visits.count() == paths.num_locations);
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

    for(const auto id : boost::irange(paths.num_locations)) {

      auto visits = state.visits;

      const auto target_not_visited = !visits.test(id);

      if(target_not_visited) {

        visits.set(id);

        const auto first = state.first;
        const auto last  = state.last;

        /*
         * A* will supply the callback function that will
         * add the next possible state (neighbor) to explore
         * to the queue
         */
        callback({first, id, visits}, paths.lookup(last, id));
        callback({id, last, visits}, paths.lookup(first, id));
      }
    }
	};

	const auto begin_state = State{0, 0, 1};

	return a_star<uintmax>(begin_state, heuristic, is_goal, for_each_neighbor).second;
}

int main() {

  std::cout << solution(puzzle_input) << std::endl;

}
