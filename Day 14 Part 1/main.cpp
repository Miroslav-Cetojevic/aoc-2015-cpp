#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Reindeer {
	std::string name;
	std::size_t speed, runtime, resttime;
};

auto& operator>>(std::istream& in, Reindeer& r) {
	return in >> r.name >> r.speed >> r.runtime >> r.resttime;
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"reindeers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto reindeers = std::vector<Reindeer>{};
		auto name = std::string{};

		std::size_t seconds;
		file >> seconds;

		Reindeer entry;
		while(file >> entry) {
			reindeers.emplace_back(entry);
		}

		auto max_distance = std::accumulate(reindeers.begin(), reindeers.end(), 0UL, [seconds] (auto sum, auto& reindeer) {
			const auto distance = (reindeer.speed * reindeer.runtime);
			const auto cycle = (reindeer.runtime + reindeer.resttime);
			const auto cycles = (seconds / cycle);
			const auto time_left = (seconds % cycle);

			auto tmp_distance = (distance * cycles);

			tmp_distance += (time_left >= reindeer.runtime) ? distance : (reindeer.speed * time_left);

			return std::max(sum, tmp_distance);
		});

		std::cout << max_distance << std::endl;
	} else {
		std::cerr << "Error! Could not open " << filename << "!" << std::endl;
	}
	return 0;
}
