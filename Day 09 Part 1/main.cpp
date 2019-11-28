#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/iterator/counting_iterator.hpp>

using Location = std::string;
using LocationID = unsigned;
using Distance = unsigned;

struct Route {
	LocationID location_id1, location_id2;
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
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

template<typename T>
auto factorial(T n) {
	auto range = Range<T>{2, (n + 1)};
	return std::accumulate(range.begin, range.end, T{1}, std::multiplies{});
}

int main() {

	auto filename = std::string{"locations.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto locations = std::set<LocationID>{};

		auto add_location = [&locations](const auto& location) {

			static auto vertex_id = LocationID{};
			static auto geo_map = std::unordered_map<Location, LocationID>{};

			auto pos = geo_map.try_emplace(location, 0);

			if(pos.second) {
				pos.first->second = vertex_id++;
				locations.insert(pos.first->second);
			}

			return pos.first->second;
		};

		auto routes = std::vector<Route>{};
		auto entry = RouteEntry{};

		while(file >> entry) {
			routes.push_back(Route{add_location(entry.location_A),
								   add_location(entry.location_B),
								   entry.distance});
		}

		/*
		 * find shortest path through all locations
		 */

		// create a lookup table for distances of all routes
		const auto size = locations.size();
		auto chart = std::vector<std::vector<Distance>>(size);

		for(auto& entry : chart) {
			entry = std::vector<Distance>(size);
		}

		// log the distance for any given route between two locations
		for(const auto& route : routes) {

			auto& id1 = route.location_id1;
			auto& id2 = route.location_id2;

			chart[id1][id2] = chart[id2][id1] = route.distance;
		}

		auto itinerary = std::vector<LocationID>{locations.begin(), locations.end()};
		auto min_distance = Distance{std::numeric_limits<Distance>::max()};

		// only a subset of all possible permutations of routes is relevant for us,
		// this will allow us to return early from looping through the permutations
		auto limit = (factorial(size) / size) * (size - 1);

		do {// inner_product is equal to acc += lambda(iter1, iter2) in a loop
			auto tmp_distance = std::inner_product(itinerary.begin(),
												   std::prev(itinerary.end()),
												   std::next(itinerary.begin()),
												   Distance{},
												   std::plus{},
												   [&chart] (auto a, auto b) { return chart[a][b]; });

			min_distance = std::min(min_distance, tmp_distance);

		} while(((limit--) > 0) && std::next_permutation(itinerary.begin(), itinerary.end()));

		std::cout << min_distance << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
