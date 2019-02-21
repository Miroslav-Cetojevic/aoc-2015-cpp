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

using Vertex = std::string;
using VertexID = std::uint64_t;
using Distance = std::uint64_t;

struct Edge {
	VertexID v, w;
	Distance d;
};

struct EdgeString {
	std::string v, w, d;
};

auto& operator>>(std::istream& in, EdgeString& es) {
	return in >> es.v >> es.w >> es.d;
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
		auto vertex_map = std::unordered_map<Vertex, VertexID>{};
		auto vertices = std::set<VertexID>{};

		auto add_vertex = [&vertex_map, &vertices](const auto& vertex) {
			static auto vertex_id = VertexID{};
			auto pos = vertex_map.try_emplace(vertex, 0);
			if(pos.second) {
				pos.first->second = vertex_id++;
				vertices.insert(pos.first->second);
			}
			return pos.first->second;
		};

		auto edges = std::vector<Edge>{};
		auto edge_str = EdgeString{};
		while(file >> edge_str) {
			edges.push_back(Edge{add_vertex(edge_str.v),
								 add_vertex(edge_str.w),
								 std::stoul(edge_str.d)});
		}

		/*
		 * find shortest path through all vertices
		 */
		auto size = vertices.size();
		auto matrix = std::vector<std::vector<Distance>>(size);
		for(auto& row : matrix) {
			row = std::vector<Distance>(size);
		}

		// populate matrix with edges
		for(const auto& edge : edges) {
			matrix[edge.v][edge.w] = matrix[edge.w][edge.v] = edge.d;
		}

		auto permutable = std::vector<VertexID>{vertices.begin(), vertices.end()};
		auto min_distance = Distance{UINT64_MAX};

		auto f = factorial(size);
		auto limit = (f / size) * (size - 1);

		do {
			auto tmp_distance = std::inner_product(permutable.begin(),
												   std::prev(permutable.end()),
												   std::next(permutable.begin()),
												   Distance{},
												   std::plus{},
												   [&matrix] (auto a, auto b) { return matrix[a][b]; });

			min_distance = std::min(min_distance, tmp_distance);

		} while((limit--) > 0 && std::next_permutation(permutable.begin(), permutable.end()));

		std::cout << min_distance << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
