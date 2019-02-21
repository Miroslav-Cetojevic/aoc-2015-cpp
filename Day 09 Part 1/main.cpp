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
using LocationID = std::size_t;
using Distance = std::size_t;

struct Route {
	LocationID location1, location2;
	Distance distance;
};

struct RouteEntry {
	std::string location1, location2;
	std::size_t distance;
};

auto& operator>>(std::istream& in, RouteEntry& re) {
	return in >> re.location1 >> re.location2 >> re.distance;
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
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"locations.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto geo_map = std::unordered_map<Location, LocationID>{};
		auto locations = std::set<LocationID>{};

		auto add_location = [&geo_map, &locations](const auto& location) {
			static auto vertex_id = LocationID{};
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
			routes.emplace_back(Route{add_location(entry.location1),
									  add_location(entry.location2),
									  entry.distance});
		}

		/*
		 * find shortest path through all locations
		 */
		auto size = locations.size();
		auto chart = std::vector<std::vector<Distance>>{size};
		for(auto& entry : chart) {
			entry = std::vector<Distance>(size);
		}

		// populate chart with routes
		for(const auto& route : routes) {
			chart[route.location1][route.location2]
			= chart[route.location2][route.location1]
			= route.distance;
		}

		auto itinerary = std::vector<LocationID>{locations.begin(), locations.end()};
		auto min_distance = Distance{std::numeric_limits<Distance>::max()};

		auto f = factorial(size);
		auto limit = (f / size) * (size - 1);

		do {
			auto tmp_distance = std::inner_product(itinerary.begin(),
												   std::prev(itinerary.end()),
												   std::next(itinerary.begin()),
												   Distance{},
												   std::plus{},
												   [&chart] (auto a, auto b) { return chart[a][b]; });

			min_distance = std::min(min_distance, tmp_distance);

		} while((limit--) > 0 && std::next_permutation(itinerary.begin(), itinerary.end()));

		std::cout << min_distance << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
