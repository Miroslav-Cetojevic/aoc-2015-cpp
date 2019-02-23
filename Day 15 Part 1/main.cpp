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

struct Ingredient {
	std::size_t id, calories;
	std::ptrdiff_t capacity, durability, flavor, texture;
};

auto& operator>>(std::istream& in, Ingredient& ingredient) {
	static auto id = 0UL;
	ingredient.id = id++;

	std::string name;
	return in >> name >> ingredient.capacity >> ingredient.durability
			  >> ingredient.flavor >> ingredient.texture >> ingredient.calories;
}

struct Properties {
    std::array<std::ptrdiff_t, 4> data{};

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }

    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    auto& operator[](std::size_t n) { return data[n]; }
    auto& operator[](std::size_t n) const { return data[n]; }
};

auto operator+(const Properties& lhs, const Properties& rhs) {
	auto P = Properties{};
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), P.begin(), std::plus{});
	return P;
}

auto operator*(const std::size_t scalar, const Ingredient& ingredient) {
	auto n = static_cast<decltype(ingredient.capacity)>(scalar);
	return Properties{n * ingredient.capacity,
					  n * ingredient.durability,
					  n * ingredient.flavor,
					  n * ingredient.texture};
}

// this function is based on this SO answer: https://stackoverflow.com/a/22989846/699211
template<typename B, typename I, typename N>
N partition(B& baskets, N n_baskets, I& ingredients, N ingredient_id, N spoons_left, N max_score) {
	if(spoons_left > 0) {

		if((ingredient_id + 1) < n_baskets) {

			auto min_spoons = ((ingredient_id == 0) ? N{1} : baskets[ingredient_id-1]);

			auto max_spoons = (spoons_left / 2);

			for(auto n_spoons = min_spoons; n_spoons <= max_spoons; ++n_spoons){
				baskets[ingredient_id] = n_spoons; /* replace last */
				max_score = partition(baskets, n_baskets, ingredients, (ingredient_id + 1), (spoons_left - n_spoons), max_score);
			}

		} else {
			baskets[ingredient_id] = spoons_left;

			auto tmp_baskets = baskets;
			auto properties = Properties{};

			do {
				properties = std::inner_product(tmp_baskets.begin(),
												tmp_baskets.end(),
												ingredients.begin(),
												properties,
												std::plus{},
												std::multiplies{});

				auto tmp_score = std::accumulate(properties.begin(), properties.end(), 1UL, [] (auto product, auto value) {
					return product *= ((value >= 0) ? value : 0);
				});

				max_score = std::max(max_score, tmp_score);

				std::fill(properties.begin(), properties.end(), 0);

			} while(std::next_permutation(tmp_baskets.begin(), tmp_baskets.end()));
		}
	}

	return max_score;
}

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"ingredients.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto ingredients = std::vector<Ingredient>{};

		Ingredient ingredient;
		while(file >> ingredient) {
			ingredients.push_back(ingredient);
		}

		auto baskets = std::vector<std::size_t>(ingredients.size());

		auto max_score = partition(baskets, baskets.size(), ingredients, 0UL, 100UL, 0UL);

		std::cout << max_score << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
