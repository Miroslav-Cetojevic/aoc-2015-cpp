#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <sstream>
#include <string>

#include "input.hpp"

using uintmax = std::uintmax_t;

struct Reindeer {
  uintmax speed,
          runtime,
          resttime;
};

auto& operator>>(std::istream& in, Reindeer& r) {
  std::string s; // throwaway input
  return in >> s >> s >> s
            >> r.speed >> s >> s
            >> r.runtime >> s >> s >> s >> s >> s >> s
            >> r.resttime >> s;
}

auto solution(const std::string& input) {

  auto stream = std::stringstream{input};

  using iterator = std::istream_iterator<Reindeer>;

  auto begin = iterator{stream};
  auto end   = iterator{};
  auto init  = uintmax{};

  const auto max = [seconds = uintmax{2503}] (const auto current_max, const auto& reindeer) {

    const auto runtime = reindeer.runtime;
    const auto speed = reindeer.speed;

    const auto distance = (speed * runtime);
    const auto cycle = (runtime + reindeer.resttime);
    const auto cycles = (seconds / cycle);
    const auto time_left = (seconds % cycle);

    // has this reindeer enough time for another full sprint before resting?
    const auto can_do_full_sprint = (time_left >= runtime);

    const auto tmp_distance = (distance * cycles)
                              + (can_do_full_sprint ? distance : (speed * time_left));

    return std::max(current_max, tmp_distance);
  };

  return std::accumulate(begin, end, init, max);
}

int main() {

	std::cout << solution(puzzle_input) << std::endl;

}
