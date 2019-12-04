#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <iterator>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/range/counting_range.hpp>

using Location = std::string;
using LocationID = unsigned;
using Distance = unsigned;

struct Route {
	LocationID id1, id2;
	Distance distance;
};

struct RouteEntry {
	Location location_A, location_B;
	Distance distance;
};

auto& operator>>(std::istream& in, RouteEntry& entry) {
	return in >> entry.location_A >> entry.location_B >> entry.distance;
}

template<typename T>
auto factorial(T n) {
	const auto range = boost::counting_range<T>(2, (n + 1));
	return std::accumulate(range.begin(), range.end(), T{1}, std::multiplies{});
}

template<typename T>
auto triangle(T n) {
	return n * (n - 1) / 2;
}

template<typename T>
auto triangle_index(T a, T b) {
	const auto& [min, max] = std::minmax(a, b);
	return min + triangle(max);
}

int main() {

	const auto filename = std::string{"locations.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto locations = std::set<LocationID>{};

		const auto add_location = [&locations] (const auto& location) {

			static auto location_id = LocationID{};
			static auto geo_map = std::unordered_map<Location, LocationID>{};

			const auto pos = geo_map.try_emplace(location, 0);

			auto& current_id = pos.first->second;

			const auto is_new_location = pos.second;

			if(is_new_location) {
				// the ids start from 0, hence the postfix increment
				current_id = location_id++;
				locations.insert(current_id);
			}

			return current_id;
		};

		auto routes = std::vector<Route>{};

		RouteEntry entry;

		while(file >> entry) {
			routes.push_back({
				add_location(entry.location_A),
				add_location(entry.location_B),
				entry.distance
			});
		}

		// ========================================
		// find shortest path through all locations
		// ========================================

		// will be used for various calculations
		const auto size = locations.size();

		// 1) creates a lookup table for distances of all routes
		// 2) doing triangle instead of (size*size) halves the memory footprint,
		// but at the expense of doing additional calculation for each access
		auto chart = std::vector<Distance>(triangle(size));

		// log the distance for any given route between two locations
		for(const auto& route : routes) {
			const auto& index = triangle_index(route.id1, route.id2);
			chart[index] = route.distance;
		}

		auto itinerary = std::vector<LocationID>{locations.begin(), locations.end()};
		auto min_distance = Distance{std::numeric_limits<Distance>::max()};

		// only a subset of all possible permutations of routes is relevant for us,
		// so we create an upper limit to count down from when looping through permutations
		auto limit = (factorial(size) / size) * (size - 1);

		do {
			const auto begin1 = itinerary.begin();
			const auto end1	  = std::prev(itinerary.end());
			const auto begin2 = std::next(begin1);
			const auto acc	  = Distance{};
			const auto op1	  = std::plus{};
			const auto op2	  = [&chart] (auto a, auto b) { return chart[triangle_index(a, b)]; };

			// inner_product resolves to acc = op1(acc, op2(begin1, begin2)) in a loop
			const auto tmp_distance = std::inner_product(begin1, end1, begin2, acc, op1, op2);

			min_distance = std::min(min_distance, tmp_distance);

		} while(((limit--) > 0) && std::next_permutation(itinerary.begin(), itinerary.end()));

		std::cout << min_distance << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
