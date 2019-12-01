#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>

struct Reindeer {
	std::string name;
	unsigned speed;
	unsigned runtime;
	unsigned resttime;
};

auto& operator>>(std::istream& in, Reindeer& r) {
	return in >> r.name >> r.speed >> r.runtime >> r.resttime;
}

int main() {

	const auto filename = std::string{"reindeers.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		unsigned seconds;

		file >> seconds;

		auto begin = std::istream_iterator<Reindeer>{file};
		auto end   = std::istream_iterator<Reindeer>{};
		auto init  = unsigned{};
		auto max   = [seconds] (const auto current_max, const auto& reindeer) {

			const auto runtime = reindeer.runtime;
			const auto speed = reindeer.speed;

			const auto distance = (speed * runtime);
			const auto cycle = (runtime + reindeer.resttime);
			const auto cycles = (seconds / cycle);
			const auto time_left = (seconds % cycle);

			auto tmp_distance = (distance * cycles);

			// has this reindeer enough time for another full sprint before resting?
			const auto can_do_full_sprint = (time_left >= runtime);

			if(can_do_full_sprint) {
				tmp_distance += distance;
			} else {
				tmp_distance += (speed * time_left);
			}

			return std::max(current_max, tmp_distance);
		};

		// std::accumulate can be used to compute the minmax or a sequence
		const auto max_distance = std::accumulate(begin, end, init, max);

		std::cout << max_distance << std::endl;

	} else {
		std::cerr << "Error! Could not open " << filename << "!" << std::endl;
	}

	return 0;
}
