// NOTE: this is a subset sum problem
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

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
		find_subset(new_subset, subsets, weights, lookup, limit-1, sum);
	}

	if(sum >= weights[limit] && lookup[limit-1][sum-weights[limit]]) {
		subset.push_back(weights[limit]);
		find_subset(subset, subsets, weights, lookup, limit-1, (sum - weights[limit]));
	}
}

template<typename T, typename N>
auto find_subsets(const T& weights, N sum) {
	auto subsets = std::vector<T>{};

	using Boolean = std::uint8_t;
	auto lookup = std::vector<std::vector<Boolean>>(weights.size());

	for(auto& entry : lookup) {
		entry = std::vector<Boolean>(sum + 1);
	}

	if (weights.front() <= sum) {
		lookup.front()[weights.front()] = true;
	}

	for(auto i = 1UL; i < weights.size(); ++i) {

		for(auto j = 0UL; j < sum + 1; ++j) {

            lookup[i][j] = ((weights[i] <= j) ?
            			   (lookup[i-1][j] || lookup[i-1][j-weights[i]]) :
						   lookup[i-1][j]);
		}
	}

	find_subset(T{}, subsets, weights, lookup, (weights.size() - 1), sum);

	std::reverse(subsets.begin(), subsets.end());

	return subsets;
}

template<typename T>
auto find_min_sets(const T& subsets) {
	auto min_size = std::numeric_limits<std::size_t>::max();
	auto min_sets = T{};

	for(const auto& subset : subsets) {

		if(subset.size() < min_size) {

			if(!min_sets.empty()) {

				// reverse-iterating through min_sets
				auto i = min_sets.size();
				while((i--) > 0) {
					if(min_sets[i].size() == min_size) {
						min_sets.pop_back();
					}
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
	auto min_value = std::numeric_limits<std::size_t>::max();

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

		auto weights = std::vector<std::size_t>{};

		auto total_weight = 0UL;

		std::size_t weight;

		while(file >> weight) {

			total_weight += weight;

			weights.push_back(weight);
		}

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
