#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include <boost/range/irange.hpp>

#include "input.hpp"

using uintmax = std::uintmax_t;

struct Reindeer {
  uintmax speed,
          runtime,
          resttime,
          total_distance = 0,
          score = 0;
};

auto& operator>>(std::istream& in, Reindeer& r) {
  std::string s; // throwaway input
  return in >> s >> s >> s
            >> r.speed >> s >> s
            >> r.runtime >> s >> s >> s >> s >> s >> s
            >> r.resttime >> s;
}

using Reindeers = std::vector<Reindeer>;

auto parse(const std::string& input) {
  auto stream = std::stringstream{input};

  using iterator = std::istream_iterator<Reindeer>;

  const auto stream_begin = iterator{stream};
  const auto stream_end   = iterator{};

  return Reindeers{stream_begin, stream_end};
}

auto calculate_scores(Reindeers& reindeers, uintmax seconds) {

  const auto cmp_distance = [] (const auto& a, const auto& b) {
    return a.total_distance < b.total_distance;
  };

  const auto timespan = boost::irange({1}, (seconds + 1));

  for(const auto second : timespan) {

    for(auto& reindeer : reindeers) {

      const auto runtime = reindeer.runtime;
      const auto cycle = (runtime + reindeer.resttime);
      const auto time_left = (second % cycle);

      // check if reindeer is running right this second
      const auto has_time_left = (time_left > 0) && (time_left <= runtime);

      if(has_time_left) {
        reindeer.total_distance += reindeer.speed;
      }
    }

    const auto leading_distance = std::max_element(reindeers.begin(), reindeers.end(), cmp_distance)->total_distance;

    // one point for each leader
    for(auto& reindeer : reindeers) {
      reindeer.score += (reindeer.total_distance == leading_distance);
    }
  }
}

auto max_score(const Reindeers& reindeers) {

  const auto cmp_score  = [] (const auto& a, const auto& b) {
    return a.score < b.score;
  };

  return std::max_element(reindeers.begin(), reindeers.end(), cmp_score)->score;
}

auto solution(const std::string& input, uintmax seconds) {

  auto reindeers = parse(input);

  calculate_scores(reindeers, seconds);

  return max_score(reindeers);
}

int main() {

  std::cout << solution(puzzle_input, 2503) << std::endl;

}
