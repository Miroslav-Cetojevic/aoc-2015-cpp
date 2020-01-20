#include <cmath>
#include <iostream>
#include <vector>

/*
 * Sum of factors is (p1^0 + p1^1 + p1^2 + ...)*(p2^0 + p2^1 + p2^2 + ...)*...
 * for all prime factors and their powers. If we find a dividing prime, we can
 * divide away one of the prime factors completely, then take the sum of factors
 * of the number that's left and multiply it with the sum of the remaining prime's
 * powers.
 */
auto solution(int total_presents) {

  /*
   * Every elf brings 10 times its number to each house, if we divide the total
   * by 10, we only need the elf's number to denote the amount of presents,
   * saving ourselves a bit of computations.
   */
  const auto total_houses = (total_presents / 10);

  auto primes = std::vector{2};

  // each house_id shows the total amount of presents it got
  auto house_ids = std::vector{0, 1, 3};
  house_ids.reserve(total_houses / 2);

  const auto num_presents = [&] (auto house_id) {

    const auto sum_of_dividing_powers = [] (auto house_id, auto prime) {
      // 1 + p + p^2 +... until p^n doesn't divide current
      auto prime_copy = prime;
      while((house_id % prime) == 0) {
        prime *= prime_copy;
      }
      return std::pair{
        ((prime-1)/(prime_copy-1)),
        (house_id/(prime/prime_copy))
      };
    };

    auto presents = 0;

    for(const auto prime : primes) {
      if(std::pow(prime, 2) > house_id) {
        primes.push_back(house_id);
        presents = (house_id + 1);
        break;
      } else if((house_id % prime) == 0) {
        const auto [new_presents, new_house] = sum_of_dividing_powers(house_id, prime);
        presents = house_ids[new_house] * new_presents;
        break;
      }
    }

    return presents;
  };

  auto house_id = 2;

  for(auto presents = 0; presents < total_houses;) {
    presents = num_presents(++house_id);
    house_ids.push_back(presents);
  }

  return house_id;
}

int main() {

  const auto total_presents = 33100000;

  std::cout << solution(total_presents) << std::endl;
}
