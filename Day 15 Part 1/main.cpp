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

	static std::string name;
	return in >> name >> ingredient.capacity >> ingredient.durability
			  >> ingredient.flavor >> ingredient.texture >> ingredient.calories;
}


// this function is based on the following SO answer:
// https://stackoverflow.com/a/22989846/699211
template<typename B, typename I, typename N>
void partition(B& baskets, N n_baskets, I& ingredients, N ingredient_id, N spoons_left, N& max_score) {
	if(spoons_left > 0) {

		if((ingredient_id + 1) < n_baskets) {

			auto min_spoons = ((ingredient_id == 0) ? 1UL : baskets[ingredient_id-1]);

			auto max_spoons = (spoons_left / 2);

			for(auto n_spoons = min_spoons; n_spoons <= max_spoons; ++n_spoons){
				baskets[ingredient_id] = n_spoons; /* replace last */
				partition(baskets, n_baskets, ingredients, (ingredient_id + 1), (spoons_left - n_spoons), max_score);
			}

		} else {
			baskets[ingredient_id] = spoons_left;

			auto tmp_baskets = baskets;
			using Properties = std::array<std::ptrdiff_t, 4>;
			auto properties = Properties{};

			auto sum_properties = [] (auto acc, auto next) {
				auto P = Properties{};
				std::transform(acc.begin(), acc.end(), next.begin(), P.begin(), std::plus{});
				return P;
			};

			auto multiply_properties = [] (auto num, auto ingredient) {
				return Properties{num * ingredient.capacity,
								  num * ingredient.durability,
								  num * ingredient.flavor,
								  num * ingredient.texture};
			};

			do {
				properties = std::inner_product(tmp_baskets.begin(),
												tmp_baskets.end(),
												ingredients.begin(),
												properties,
												sum_properties,
												multiply_properties);

				auto tmp_score = std::accumulate(properties.begin(), properties.end(), 1UL, [] (auto product, auto value) {
					return product *= ((value >= 0) ? value : 0);
				});

				max_score = std::max(max_score, tmp_score);

				std::fill(properties.begin(), properties.end(), 0);

			} while(std::next_permutation(tmp_baskets.begin(), tmp_baskets.end()));
		}
	}
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

		auto ingredient_id = 0UL;
		auto spoons_left = 100UL;
		auto max_score = 0UL;

		partition(baskets, baskets.size(), ingredients, ingredient_id, spoons_left, max_score);

		std::cout << max_score << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
