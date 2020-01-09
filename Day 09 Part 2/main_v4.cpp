#include <algorithm>
#include <array>
#include <bitset>
#include <charconv>
#include <iostream>
#include <numeric>
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

struct ParseStats {
  uintmax num_locations;
  uintmax num_entries;
};

constexpr auto space = ' ';
constexpr auto newline = '\n';

// compile-time parsing of string_view
constexpr auto parse_stats(std::string_view input) {
  auto stats = ParseStats{};
  auto& num_locations = stats.num_locations;
  auto& num_entries = stats.num_entries;

  const auto starts_with = [&input] (auto substring) {
    return input.substr(0, substring.size()) == substring;
  };

  const auto end = input.find_first_of(space) + 1;
  const auto first_place = input.substr(0, end);

  while(starts_with(first_place)) {
    ++num_locations;
    input.remove_prefix(input.find_first_of(newline) + 1);
    ++num_entries;
  }

  for(const auto c : input) {
    num_entries += (c == newline);
  }

  return stats;
}

constexpr auto PARSE_STATS = parse_stats(puzzle_input);
constexpr auto NUM_ENTRIES = (PARSE_STATS.num_entries + 1);
constexpr auto NUM_LOCATIONS = (PARSE_STATS.num_locations + 1);

using Visits = std::bitset<NUM_LOCATIONS>;

/*
 * Diagonal matrix
 *
 * (1,0)
 * (2,0), (2,1)
 * (3,0), (3,1), (3,2)
 */
struct AdjacencyMatrix {

  uintmax num_locations = NUM_LOCATIONS;
  std::array<Distance, NUM_ENTRIES> matrix;

//  std::vector<Distance> matrix;
//  AdjacencyMatrix() {
//    matrix.reserve(NUM_ENTRIES);
//  }

  auto lookup(LocationID x, LocationID y) const {
    const auto triangle = [] (auto n) {
        return (n * (n - 1)) / 2;
    };
    const auto [min, max] = std::minmax(x, y);
    return matrix[triangle(max) + min];
  }
};

constexpr auto split_input(std::string_view input) {

  auto lines = std::array<std::string_view, NUM_ENTRIES>{};

  for(auto& line : lines) {
    const auto pos = input.find(newline);
    line = input.substr(0, pos);
    input.remove_prefix(pos + 1);
  }

  return lines;
}

constexpr auto to_int(std::string_view input) {
  uintmax result = 0;
  for(auto c : input) {
    result *= 10;
    result += c - '0';
  }
  return result;

}

template<typename Iterator>
constexpr void reverse(Iterator first, Iterator last) {
  while ((first != last) && (first != --last)) {
    std::iter_swap(first++, last);
  }
}

constexpr auto parse_input(std::string_view input) {

  auto paths = AdjacencyMatrix{};

  auto& matrix = paths.matrix;

  auto i = 0;
  for(auto line : split_input(input)) {
    const auto last_token = line.find_last_of(space) + 1;
    const auto distance = to_int(line.substr(last_token));
    matrix[i++] = distance;
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
   * We end up with an ordering that would translate to:
   *
   * (3,2) (3,1) (3,0)
   * (2,1) (2,0)
   * (1,0)
   *
   * which won't work for an adjacency matrix. So we reverse the array
   * instead.
   */
  reverse(matrix.begin(), matrix.end());

  return paths;
}

struct State {
  uintmax first, last;
  Visits visits;
  Distance distance;

  /*
   * Always make sure the locations (first and last) are sorted by id so we can
   * avoid creating equivalent states, thus reducing the search space in half.
   *
   * E.g. A->B->C is equivalent to C->B->A.
   */
  State(uintmax f, uintmax l, Visits v, Distance d = {}):
    first(std::min(f, l)),
    last(std::max(f, l)),
    visits(v),
    distance(d) {};

  // this will be called by the comparator of the priority queue in the A* search algorithm
  auto operator<(const State& other) const {
    const auto lhs = visits.to_ulong();
    const auto rhs = other.visits.to_ulong();
    return std::tie(first, last, lhs) < std::tie(other.first, other.last, rhs);
  }

  // this is required so we can use State as a key in an unordered map
  auto operator==(const State& other) const {
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
template<typename Score, typename Heuristic, typename Goal, typename Next>
auto a_star(const State& start, Heuristic&& heuristic, Goal&& is_goal, Next&& for_each_neighbor) {

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
    {start, 0}
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

/*
 * Adapted from https://stackoverflow.com/a/33181173
 *
 * Heap tracks the minimum k values.
 *
 * If the internal storage space is full, it will do a heap
 * sort which puts the maximum element in the heap at the top.
 *
 * A new value will only be inserted if it's smaller than the
 * top element. On insertion, the top element will be popped
 * off the heap, the new element inserted, and the next max
 * element put at the top.
 *
 */
template<typename T = double, typename U = uintmax>
class Heap {
  private:
    U k;
  public:
    std::vector<T> container;

    Heap(U n) : k(n) {
        container.reserve(k);
    }

    void insert(T value) {
      const auto begin = container.begin();
      const auto end   = container.end();

      if(container.size() < k) {

        container.push_back(value);

        if (container.size() == k) {
          std::make_heap(begin, end);
        }

      } else if(value < container.front()) {
        // this is the equivalent of a heap_swift_down
        std::pop_heap(begin, end);
        container.back() = value;
        std::push_heap(begin, end);
      }
    }
};

auto solution(std::string_view input) {

  const auto paths = parse_input(input);

  const auto lookup = [&paths] (auto x, auto y) {
    return paths.lookup(x, y);
  };

  const auto inverse = [] (auto x) {
    return (1.0 / x);
  };

  /*
   * Estimates the lowest cost from the current location (State::first) to the goal.
   *
   * Note: A* with a heuristic of zero is equivalent to Dijkstra's search.
   */
  const auto heuristic = [&] (const auto& state) {

    auto result = Distance{};

    const auto visits = state.visits;
    const auto num_visits  = visits.count();
    const auto num_locations = paths.num_locations;

    if(num_visits < num_locations) {
      auto mins = Heap{num_locations - num_visits - 1};

      // every state has visited the 0th location, because they have to start somewhere
      // i represents the first endpoint of a path
      for(const auto first : boost::irange({1}, num_locations)) {

        if(not visits.test(first)) {

          // j represents the other endpoint of a path
          for(const auto last : boost::irange(first)) {

            if(not visits.test(last)) {
              mins.insert(inverse(lookup(first, last)));
            }
          }
        }
      }
      const auto& heap = mins.container;
      result = std::accumulate(heap.begin(), heap.end(), Distance{});
    }
    return result;
  };

  const auto is_goal = [&paths] (const auto& state) {
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
  const auto for_each_neighbor = [&] (const auto& state, auto&& callback) {

    for(const auto id : boost::irange(paths.num_locations)) {

      auto visits = state.visits;

      const auto target_not_visited = (not visits.test(id));

      if(target_not_visited) {

        visits.set(id);

        const auto first = state.first;
        const auto last  = state.last;

        const auto distance = state.distance;

        const auto new_distance = [&] (auto location) {
          return (lookup(location, id) + distance);
        };

        const auto last_distance  = new_distance(last);
        const auto first_distance = new_distance(first);

        /*
         * A* will supply the callback function that will
         * add the next possible state (neighbor) to explore
         * to the queue
         */
        callback({first, id, visits, last_distance}, inverse(lookup(last, id)));
        callback({id, last, visits, first_distance}, inverse(lookup(first, id)));
      }
    }
  };

  const auto begin_state = State{0, 0, 1};

  return a_star<double>(begin_state, heuristic, is_goal, for_each_neighbor).first.distance;
}

int main() {

  std::cout << solution(puzzle_input) << std::endl;

}
