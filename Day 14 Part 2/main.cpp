#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
class Range {
	private:
		boost::counting_iterator<T, boost::use_default, T> begin_, end_;
	public:
		Range(T b, T e): begin_(b), end_(e) {}
		auto begin() const { return begin_; }
		auto end() const { return end_; }
};

struct Reindeer {
	std::string name;
	unsigned speed;
	unsigned runtime;
	unsigned resttime;
	unsigned total_distance;
	unsigned score;
};

auto& operator>>(std::istream& in, Reindeer& r) {
	in >> r.name >> r.speed >> r.runtime >> r.resttime;
	r.total_distance = r.score = 0;
	return in;
}

auto operator>(const Reindeer& lhs, const Reindeer& rhs) {
	return (lhs.total_distance > rhs.total_distance);
}

template<typename R, typename N>
auto calculate_scores(R& reindeers, N seconds) {

	auto timespan = Range<unsigned>{1, (seconds + 1)};

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

		// update the ranking in total distance
		std::sort(reindeers.begin(), reindeers.end(), std::greater<Reindeer>{});

		// one point for each leader
		for(auto& reindeer : reindeers) {
			if(reindeer.total_distance == reindeers.front().total_distance) {
				++(reindeer.score);
			}
		}
	}
}

template<typename R>
auto get_max_score(const R& reindeers) {

	const auto begin = reindeers.begin();
	const auto end   = reindeers.end();
	const auto init  = unsigned{};
	const auto max   = [] (auto current_max, auto& reindeer) {
		return std::max(current_max, reindeer.score);
	};

	// std::accumulate can be used to compute the minmax or a sequence
	return std::accumulate(begin, end, init, max);
}

int main() {

	const auto filename = std::string{"reindeers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto reindeers = std::vector<Reindeer>{};

		unsigned seconds;

		file >> seconds;

		Reindeer entry;

		while(file >> entry) {
			reindeers.emplace_back(entry);
		}

		calculate_scores(reindeers, seconds);

		std::cout << get_max_score(reindeers) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
