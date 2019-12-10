// NOTE: this is a subset sum problem
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/irange.hpp>

// this recursive function will go through all possible combinations of weights, starting with the last weight in the list,
// which is indicated by parameter 'last', until the sum (or what's left of it) is reached
template<typename Set, typename Sets, typename W, typename T, typename N>
auto find_subset(Set&& subset, Sets&& subsets, const W& weights, const T& tbl_has_sum, N row, N sum_left) -> void {

	const auto only_one_element_left = (row == 0);

	// first base case: only one weight left (first of the weights), sum is non-zero
	// and the final weight to be added to reach 'sum_left' is the the first weight
	if(only_one_element_left && (sum_left > 0) && tbl_has_sum[row][sum_left]) {

		subset.push_back(weights[row]);

		subsets.push_back(subset);

	} else if(only_one_element_left && (sum_left == 0)) {

		// second base case: only one weight left, there's no more sum left
		// to add to, so the current subset can be added to the collection
		subsets.push_back(subset);

	} else {

		const auto prev_row = (row - 1);

		// first recursive case: the matrix tells us whether or not sum_left
		// can be formed without the current weight but with all previous weights
		if(tbl_has_sum[prev_row][sum_left]) {

			// there are potentially two successive calls to find_subset, which means the passed subset will be modified
			// both times - but on the same recursion level we need the subset to be the same, so the first call will get
			// a copy, and when the recursion for that copy is finished, which at that point will be modified, we can continue
			// to the second call with the original that we started the recursion with
			auto new_subset = subset;

			find_subset(new_subset, subsets, weights, tbl_has_sum, prev_row, sum_left);
		}

		const auto weight = weights[row];

		const auto new_sum_left = (sum_left - weight);

		// second recursive case: to make access to the matrix safe, we need to check first,
		// if the current weight can be added without going over the sum we need - if the first
		// check is successful, the matrix will tell us, whether or not the previous row can
		// create the sum 'sum_left - weight' - if the answer is 'yes', we can add the current
		// weight to the current subset and make another recursive call
		if((sum_left >= weight) && tbl_has_sum[prev_row][new_sum_left]) {

			subset.push_back(weight);

			find_subset(subset, subsets, weights, tbl_has_sum, prev_row, new_sum_left);
		}
	}
}

// this function is based on: https://stackoverflow.com/a/45427013/699211
template<typename T, typename N>
auto find_subsets(const T& weights, N sum) {

	auto subsets = std::vector<T>{};

	// in dynamic programming, the first row of the lookup matrix
	// represents zero elements (empty subsets), so normally such
	// a matrix would have (n + 1) rows, but since we won't do anything
	// with that particular row, we might just as well stick to n rows
	const auto rows = weights.size();

	using VectorBool = std::vector<std::uint8_t>;
	using Matrix = std::vector<VectorBool>;

	// todo: rename lookup

	// this is a truth table - matrix[i][s] returns true or false to indicate the answer to the question:
	// "using the first i items in the array can we find a subset sum to s?"
	auto tbl_has_sum = Matrix(rows);

	const auto columns = (sum + 1);

	for(auto& row : tbl_has_sum) {
		row = VectorBool(columns);
		// first column represents subset sum = 0, which is true for all sets
		row.front() = true;
	}

	// first element of the weights container
	const auto first = weights.front();

	// if the first element of weights is <= sum, then the set containing that weight will have it as subset sum,
	// allowing to check if it's possible to combine that weight with any of the subset sums of the previous row
	// to see if we can create the subset sum we are looking for
	if (first <= sum) {
		tbl_has_sum.front()[first] = true;
	}

	for(const auto row : boost::irange({1}, rows)) {

		for(const auto col : boost::irange({}, columns)) {

			auto& cell = tbl_has_sum[row][col];

			const auto weight = weights[row];
			const auto& prev_row = tbl_has_sum[row-1];
			const auto cell_prev_row = prev_row[col];

			// ensure we don't accidentally go out of bounds
			if(weight <= col) {
				// this checks if the subset sum 'col' can be formed by
				// a) the previous set of weights (cell_prev_row) or
				// b) if subset sum 'col-weight' is formable by the previous set
				// this is the implementation of the following logic:
				// set X with subset-sum k has subset-sum k1 if subset X1 has subset-sum (k - k1)
				cell = (cell_prev_row || prev_row[col-weight]);
			} else {
				cell = cell_prev_row;
			}
		}
	}

	// start recursion
	find_subset(T{}, subsets, weights, tbl_has_sum, (rows - 1), sum);

	return subsets;
}

template<typename T>
auto find_min_sets(const T& subsets) {

	using value_type = decltype(subsets.size());
	auto min_size = std::numeric_limits<value_type>::max();

	auto min_sets = T{};

	for(const auto& subset : subsets) {

		const auto subset_size = subset.size();

		// we don't need any sets that have size > min_size
		// so if a smaller set is found, the min_sets collection
		// can immediately be replaced
        if(subset_size < min_size) {
        	min_size = subset_size;
        	min_sets = {subset};
        } else if(subset_size == min_size) {
        	min_sets.push_back(subset);
        }
	}

	return min_sets;
}

// =======================================
// SOME TEMPLATE NONSENSE FOR DEDUCING THE
// INNERMOST TYPE OF A NESTED CONTAINER
// =======================================
template<typename T, typename = std::void_t<>>
struct innermost_value_type {
    using type = T;
};

template<typename T>
struct innermost_value_type<T, std::void_t<typename T::value_type>> {
    using type = typename innermost_value_type<typename T::value_type>::type;
};

template<typename T>
using innermost_value_type_t = typename innermost_value_type<T>::type;
// =======================================
// END OF TEMPLATE NONSENSE
// =======================================

template<typename T>
auto find_min_quantum_entanglement(const T& min_sets) {

	using ValueType = innermost_value_type_t<T>;

	auto min_value = std::numeric_limits<ValueType>::max();

	for(const auto& min_set : min_sets) {

		const auto begin = min_set.begin();
		const auto end   = min_set.end();
		const auto init  = ValueType{1};
		const auto binop = std::multiplies{};

		const auto new_value = std::accumulate(begin, end, init, binop);

		min_value = std::min(min_value, new_value);
	}

	return min_value;
}

int main() {

	const auto filename = std::string{"weights.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto weights = std::vector<std::uint64_t>{};

		using NumericType = innermost_value_type_t<decltype(weights)>;

		using stream_iterator = std::istream_iterator<NumericType>;

		const auto begin = stream_iterator{file};
		const auto end   = stream_iterator{};
		const auto init  = NumericType{};
		const auto binop = [&weights] (auto acc, auto weight) {
			weights.push_back(weight);
			return (acc + weight);
		};

		const auto total_weight = std::accumulate(begin, end, init, binop);

		const auto n_groups = 3;

		const auto sum_per_group = (total_weight / n_groups);

		std::sort(weights.begin(), weights.end());

		const auto subsets = find_subsets(weights, sum_per_group);

		const auto min_sets = find_min_sets(subsets);

		std::cout << find_min_quantum_entanglement(min_sets) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
