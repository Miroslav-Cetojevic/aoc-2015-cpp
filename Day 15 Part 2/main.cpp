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

	std::string name; // to be discarded, as we will only use its id instead

	in >> name >> ingredient.capacity >> ingredient.durability
	   >> ingredient.flavor >> ingredient.texture >> ingredient.calories;

	return in;
}

struct Properties {
    std::array<std::int64_t, 5> data{};

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }

    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

    auto& back() { return data.back(); }

    auto& operator[](std::uint64_t n) { return data[n]; }
    auto& operator[](std::uint64_t n) const { return data[n]; }
};

auto operator+(const Properties& lhs, const Properties& rhs) {

	auto new_properties = Properties{};

	// element-wise addition of lhs and rhs
	std::transform(lhs.begin(), lhs.end(), rhs.begin(), new_properties.begin(), std::plus{});

	return new_properties;
}

auto operator*(const std::uint64_t scalar, const Ingredient& ingredient) {

	const auto scalar_signed = static_cast<std::int64_t>(scalar);

	return Properties{
		scalar_signed * ingredient.capacity,
		scalar_signed * ingredient.durability,
		scalar_signed * ingredient.flavor,
		scalar_signed * ingredient.texture,
		scalar_signed * static_cast<std::int64_t>(ingredient.calories)
	};
}

// this function is based on this stackoverflow.com answer: https://stackoverflow.com/a/22989846/699211
template<typename B, typename I, typename N>
auto partition(B& baskets, I& ingredients, N ingredient_id, N spoons_left, N calories, N max_score) -> N {

	if(spoons_left > 0) {

		if((ingredient_id + 1) < baskets.size()) {

			auto min_spoons = ((ingredient_id == 0) ? N{1} : baskets[ingredient_id-1]);

			auto max_spoons = (spoons_left / 2) + 1;

			for(auto n_spoons = min_spoons; n_spoons < max_spoons; ++n_spoons) {

				const auto new_id = (ingredient_id + 1);
				const auto new_spoons_left = (spoons_left - n_spoons);

				baskets[ingredient_id] = n_spoons;

				max_score = partition(baskets, ingredients, new_id, new_spoons_left, calories, max_score);
			}

		} else {

			baskets[ingredient_id] = spoons_left;

			auto tmp_baskets = baskets;

			do {
				const auto begin1 = tmp_baskets.begin();
				const auto end1	  = tmp_baskets.end();
				const auto begin2 = ingredients.begin();
				const auto init	  = Properties{};
				const auto op1	  = std::plus{};
				const auto op2	  = std::multiplies{};

				auto new_properties = std::inner_product(begin1, end1, begin2, init, op1, op2);

				if(new_properties.back() == static_cast<std::int64_t>(calories)) {

					const auto begin = new_properties.begin();
					const auto end   = std::prev(new_properties.end()); // calories don't factor in the score
					const auto init  = N{1};
					const auto op	 = [] (auto product, auto value) {
						return product *= ((value >= 0) ? value : 0);
					};

					auto tmp_score = std::accumulate(begin, end, init, op);

					max_score = std::max(max_score, tmp_score);
				}

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

		auto baskets = std::vector<std::size_t>(ingredients.size());

		const auto init_id = std::uint64_t{};
		const auto spoons = std::uint64_t{100};
		const auto calories = std::uint64_t{500};
		const auto init_score = std::uint64_t{};

		const auto max_score = partition(baskets, ingredients, init_id, spoons, calories, init_score);

		std::cout << max_score << std::endl;

	} else {
		std::cerr << "Error! Could not open " << filename << "!" << std::endl;
	}

	return 0;
}
