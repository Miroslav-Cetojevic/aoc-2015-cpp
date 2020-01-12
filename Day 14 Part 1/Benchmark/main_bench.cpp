#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <sstream>
#include <fstream>
#include "input.hpp"

struct Reindeer {
	unsigned speed;
	unsigned runtime;
	unsigned resttime;
};

auto& operator>>(std::istream& in, Reindeer& r) {
  std::string s; // throwaway
	return in >> s >> s >> s
	       >> r.speed >> s >> s
	       >> r.runtime >> s >> s >> s >> s >> s >> s
	       >> r.resttime >> s;
}

auto solution_miro(const std::string& input) {

  auto stream = std::stringstream{input};

  using iterator = std::istream_iterator<Reindeer>;

  auto begin = iterator{stream};
  auto end   = iterator{};
  auto init  = unsigned{};
  auto max   = [seconds = unsigned{2503}] (const auto current_max, const auto& reindeer) {

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

    // has this reindeer enough time for another full sprint before resting?
//    const auto can_do_full_sprint = (time_left >= runtime);
//
//    if(can_do_full_sprint) {
//      tmp_distance += distance;
//    } else {
//      tmp_distance += (speed * time_left);
//    }

    return std::max(current_max, tmp_distance);
  };

  return std::accumulate(begin, end, init, max);
}

std::vector<std::string_view> split(std::string_view in, char delim) {
    std::vector<std::string_view> ret;
    std::size_t pos = 0;
    while((pos = in.find(delim)) != std::string_view::npos) {
        ret.push_back(in.substr(0,pos));
        in.remove_prefix(pos+1);
    }
    ret.push_back(in);
    return ret;
}

#include <charconv>
int to_int(std::string_view s) {
    int ret;
    std::from_chars(s.begin(),s.end(),ret);
    return ret;
}

struct ReindeerSeph {
    std::string_view name;
    int speed;
    int time;
    int rest_time;
};

auto parse(std::string_view input) {
    std::vector<ReindeerSeph> ret;
    for(auto l : split(input,'\n')) {
        auto comp = split(l,' ');
        auto name = comp[0];
        auto speed = to_int(comp[3]);
        auto time = to_int(comp[6]);
        auto rest = to_int(comp[comp.size()-2]);
        ret.push_back({name,speed,time,rest});
//        std::cout << name << ' ' << speed << ' ' << time << ' ' << rest << '\n';
    }
    return ret;
}

int part1(ReindeerSeph r, int T) {
    return T/(r.rest_time+r.time)*r.speed*r.time + std::min(T % (r.rest_time+r.time),r.time)*r.speed;
}

auto solution_seph(std::string_view input) {
    auto reindeer = parse(input);
    int max = 0;
    for(auto r: reindeer) {
        max = std::max(part1(r,2503),max);
    }
    return max;
}

struct ReindeerRietty {
  std::string name;
  int speed, time, rest, points, distance;
};

using Sleigh = std::vector<ReindeerRietty>;

Sleigh parse_rietty(std::istream& input) {
  Sleigh sleigh;
  std::string n;
  int s, t, r;
  while(input >> n >> s >> t >> r) {
    ReindeerRietty deer = {n, s, t, r, 0, 0};
    sleigh.push_back(deer);
  }
  return sleigh;
}

int distance(const ReindeerRietty &deer, int second) {
  return (second / (deer.rest + deer.time) * deer.speed * deer.time) + (std::min(second % (deer.time + deer.rest), deer.time) * deer.speed);
}

auto solution_rietty(const std::string& puzzle) {
  std::stringstream input(puzzle);
  auto sleigh = parse_rietty(input);
  int furthest = 0;
  for(int i = 1; i < 2504; i++) {
    for(int d = 0; d < sleigh.size(); d++) {
      sleigh[d].distance = distance(sleigh[d], i);
      if(sleigh[d].distance > furthest) {
        furthest = sleigh[d].distance;
      }
    }
  }
  return furthest;
}

#include <chrono>
template<typename F>
double time(F&& f, std::size_t num_tries = 1) {
  const auto now = [] { return std::chrono::steady_clock::now(); };
  const auto start = now();
  for(decltype(num_tries) i = {}; i < num_tries; ++i) {
    f();
  }
  const auto end = now();
  const auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  return diff.count() / num_tries;
}

int main() {

  std::cout << solution_miro(miro_input) << std::endl;
  std::cout << solution_seph(seph_input) << std::endl;
  std::cout << solution_rietty(rietty_input) << std::endl;

  constexpr auto N = 100000;

  std::cout << time([] {
    solution_miro(miro_input);
  }, N) << std::endl;

  std::cout << time([] {
    solution_seph(seph_input);
  }, N) << std::endl;

  std::cout << time([] {
    solution_rietty(rietty_input);
  }, N) << std::endl;

	return 0;
}
