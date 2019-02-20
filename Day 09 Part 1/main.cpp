#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using Vertex = std::string;
using VertexID = std::uint64_t;
using Distance = std::uint64_t;

struct Edge {
	VertexID v;
	VertexID w;
	Distance d;
};

struct EdgeString {
	std::string v;
	std::string w;
	std::string d;
};

auto& operator>>(std::istream& in, EdgeString& es) {
	return in >> es.v >> es.w >> es.d;
}

template<typename T>
T factorial(T n) { return (n <= 1) ? 1 : factorial(n-1) * n; }

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
		auto limit = (f/size) * (size-1);

		auto i = std::uint64_t{};
		do {
			auto tmp_distance = Distance{};
			for(auto prev = permutable.begin(), curr = std::next(prev); curr != permutable.end(); ++prev, ++curr) {
				tmp_distance += matrix[*prev][*curr];
			}
			min_distance = std::min(min_distance, tmp_distance);

		} while(i++ < limit && std::next_permutation(permutable.begin(), permutable.end()));

		std::cout << min_distance << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
