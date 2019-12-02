/*
 * NOTE: this is an integer partition problem, combined with stars-and-bars
 * (both are part of combinatorial math)
 */
#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
class Range {
	private:
		boost::counting_iterator<T, boost::use_default, T> begin_, end_;
	public:
		Range(T b, T e): begin_(b), end_(e) {}
		auto begin() const { return begin_; }
		auto end() const { return end_; }
};

struct Ingredient {
	std::uint64_t id;
	std::uint64_t calories;

	std::int64_t capacity;
	std::int64_t durability;
	std::int64_t flavor;
	std::int64_t texture;
};

auto& operator>>(std::istream& in, Ingredient& ingredient) {

	static decltype(ingredient.id) id = 0;
	ingredient.id = id++;

	// to be discarded, as we will only use its id
	std::string name;

	in >> name >> ingredient.capacity >> ingredient.durability
	   >> ingredient.flavor >> ingredient.texture >> ingredient.calories;

	return in;
}

struct Properties {
    std::array<std::int64_t, 4> data{};

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }

    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    auto& operator[](std::uint64_t n) { return data[n]; }
    auto& operator[](std::uint64_t n) const { return data[n]; }
};

auto operator+(const Properties& lhs, const Properties& rhs) {

	auto properties = Properties{};

	// use std::transform for element-wise addition of lhs and rhs
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), properties.begin(), std::plus{});

	return properties;
}

auto operator*(const std::uint64_t scalar, const Ingredient& ingredient) {

	const auto scalar_signed = static_cast<std::int64_t>(scalar);

	return Properties{
		scalar_signed * ingredient.capacity,
		scalar_signed * ingredient.durability,
		scalar_signed * ingredient.flavor,
		scalar_signed * ingredient.texture
	};
}

// this function is based on this SO answer: https://stackoverflow.com/a/22989846/699211
template<typename B, typename I, typename N>
auto partition(B& baskets, I& ingredients, N ingredient_id, N spoons_left, N max_score) -> N {
	static auto counter = std::uint64_t{};
	std::cout << "partition() call no. " << (++counter) << std::endl;

	if(spoons_left > 0) {

		if((ingredient_id + 1) < baskets.size()) {

			const auto min_spoons = ((ingredient_id == 0) ? N{1} : baskets[ingredient_id-1]);

			const auto max_spoons = (spoons_left / 2) + 1;
			// TODO debug range-for
			auto range = Range{min_spoons, max_spoons};

			for(const auto n_spoons : range) {
//				std::cout << n_spoons << std::endl;
				baskets[ingredient_id] = n_spoons;

				max_score = partition(baskets,
									  ingredients,
									  (ingredient_id + 1),
									  (spoons_left - n_spoons),
									  max_score);
			}

//			for(auto n_spoons = min_spoons; n_spoons < max_spoons; ++n_spoons) {
//				std::cout << n_spoons << std::endl;
//				baskets[ingredient_id] = n_spoons;
//
//				max_score = partition(baskets,
//									  ingredients,
//									  (ingredient_id + 1),
//									  (spoons_left - n_spoons),
//									  max_score);
//			}

		} else {

			baskets[ingredient_id] = spoons_left;

			auto tmp_baskets = baskets;

			do {
				const auto properties = std::inner_product(
					tmp_baskets.begin(),
					tmp_baskets.end(),
					ingredients.begin(),
					Properties{},
					std::plus{},
					std::multiplies{});

				const auto tmp_score = std::accumulate(properties.begin(), properties.end(), N{1}, [] (auto product, auto value) {
					return product *= ((value >= 0) ? value : 0);
				});

				max_score = std::max(max_score, tmp_score);

			} while(std::next_permutation(tmp_baskets.begin(), tmp_baskets.end()));
		}
	}

	return max_score;
}

int main() {

	const auto filename = std::string{"ingredients.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto ingredients = std::vector<Ingredient>{};

		Ingredient ingredient;

		while(file >> ingredient) {
			ingredients.push_back(ingredient);
		}

		auto baskets = std::vector<std::uint64_t>(ingredients.size());

		const auto max_score = partition(baskets, ingredients, std::uint64_t{}, std::uint64_t{100}, std::uint64_t{});

		std::cout << max_score << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
