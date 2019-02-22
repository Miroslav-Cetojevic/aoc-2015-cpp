#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

struct Reindeer {
	std::string name;
	std::size_t speed, runtime, resttime, total_distance, score;
};

auto& operator>>(std::istream& in, Reindeer& r) {
	return in >> r.name >> r.speed >> r.runtime >> r.resttime;
}

auto operator>(const Reindeer& lhs, const Reindeer& rhs) {
	return (lhs.total_distance > rhs.total_distance);
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"reindeers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto reindeers = std::vector<Reindeer>{};

		std::size_t seconds;
		file >> seconds;

		Reindeer entry;
		while(file >> entry) {
			entry.total_distance = entry.score = 0;
			reindeers.emplace_back(entry);
		}

		auto range = Range<std::size_t>{1, (seconds + 1)};
		std::for_each(range.begin, range.end, [&reindeers] (auto i) {

			for(auto& reindeer : reindeers) {
				const auto cycle = (reindeer.runtime + reindeer.resttime);
				const auto time_left = (i % cycle);

				const auto has_time_left = (time_left > 0 && time_left <= reindeer.runtime);

				if(has_time_left) {
					reindeer.total_distance += reindeer.speed;
				}
			}

			std::sort(reindeers.begin(), reindeers.end(), std::greater<Reindeer>{});

			for(auto& reindeer : reindeers) {
				if(reindeer.total_distance == reindeers.front().total_distance) {
					++(reindeer.score);
				}
			}
		});

		auto max_score = std::accumulate(reindeers.begin(), reindeers.end(), 0UL, [] (auto current_max, auto& reindeer) {
			return std::max(current_max, reindeer.score);
		});

		std::cout << max_score << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
