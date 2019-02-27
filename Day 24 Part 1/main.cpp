// NOTE: this is a subset sum problem
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

template <typename T>
struct ReverseIterator {
	T& container;

    ReverseIterator(T&& container) : container(container) {}

    auto begin() { return container.rbegin(); }
    auto end() { return container.rend(); }
};

template <typename T>
auto reverse(T&& container) {
    return ReverseIterator<T>(container);
}

template<typename Set, typename Sets, typename W, typename L, typename N>
auto find_subset(Set&& subset, Sets&& subsets, W&& weights, L&& lookup, N limit, N sum) {

	if(limit == 0 && sum > 0 && lookup[limit][sum]) {
		subset.push_back(weights[limit]);
		subsets.push_back(subset);
		return;
	}

	if(limit == 0 && sum == 0) {
		subsets.push_back(subset);
		return;
	}

	if(lookup[limit-1][sum]) {
		auto new_subset = subset;
		find_subset(new_subset, subsets, weights, lookup, (limit - 1), sum);
	}

	if(sum >= weights[limit] && lookup[limit-1][sum-weights[limit]]) {
		subset.push_back(weights[limit]);
		find_subset(subset, subsets, weights, lookup, (limit - 1), (sum - weights[limit]));
	}
}

template<typename T, typename N>
auto find_subsets(const T& weights, N sum) {
	auto subsets = std::vector<T>{};

	using Boolean = std::uint8_t;
	auto lookup = std::vector<std::vector<Boolean>>(weights.size());

	auto entry_size = (sum + 1);
	for(auto& entry : lookup) {
		entry = std::vector<Boolean>(entry_size);
		entry.front() = true;
	}

	if (weights.front() <= sum) {
		lookup.front()[weights.front()] = true;
	}

	for(auto i = 1UL; i < weights.size(); ++i) {

		for(auto j = 0UL; j < entry_size; ++j) {

            lookup[i][j] = (weights[i] <= j) ?
            			   (lookup[i-1][j] || lookup[i-1][j-weights[i]]) :
						   lookup[i-1][j];
		}
	}

	// recursive function
	find_subset(T{}, subsets, weights, lookup, (weights.size() - 1), sum);

	return subsets;
}

template<typename T>
auto find_min_sets(const T& subsets) {
	auto min_size = std::numeric_limits<std::uintmax_t>::max();
	auto min_sets = T{};

	for(const auto& subset : subsets) {

		if(subset.size() < min_size) {

			if(!min_sets.empty()) {

				for(const auto& min_set : reverse(min_sets)) {
					if(min_set.size() == min_size) { min_sets.pop_back(); }
				}
			}

			min_size = subset.size();
			min_sets.push_back(subset);

		} else if(subset.size() == min_size) {
			min_sets.push_back(subset);
		}
	}

	return min_sets;
}

template<typename T>
auto find_min_quantum_entanglement(const T& min_sets) {
	auto min_value = std::numeric_limits<std::uintmax_t>::max();

	for(const auto& set : min_sets) {
		auto product = std::accumulate(set.begin(), set.end(), 1UL, std::multiplies<>());
		min_value = std::min(min_value, product);
	}

	return min_value;
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"weights.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto weights = std::vector<std::uintmax_t>{};

		auto total_weight = std::accumulate(std::istream_iterator<std::uintmax_t>{file}, {}, 0UL, [&weights] (auto acc, auto weight) {
			weights.push_back(weight);
			return (acc + weight);
		});

		auto n_groups = 3;
		auto sum_per_group = (total_weight / n_groups);

		auto subsets = find_subsets(weights, sum_per_group);

		auto min_sets = find_min_sets(subsets);

		auto min_qe = find_min_quantum_entanglement(min_sets);

		std::cout << min_qe << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
