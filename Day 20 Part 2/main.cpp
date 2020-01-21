#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/range/irange.hpp>

auto solution(int total_presents) {

  const auto total_houses = (total_presents / 11);
  const auto final_house = 50;

  /*
   * A std::array of size 3+ million elements is too much
   * for the stack, so std::vector is used here instead
   */
  auto houses = std::vector<decltype(total_presents)>(total_houses);

  auto min_house = total_houses;

  for(const auto elf_id : boost::irange(2, (total_houses))) {

    if(houses[elf_id] + elf_id >= total_houses){
      min_house = elf_id;
      break;
    }

    for(const auto house_id : boost::irange(1, (final_house + 1))) {

      const auto presents = elf_id * house_id;

      if(presents >= total_houses) {
        break;
      }

      houses[presents] += elf_id;
    }
  }

  return min_house;
}

int main() {

  const auto total_presents = 33100000;

  std::cout << solution(total_presents) << std::endl;

}
