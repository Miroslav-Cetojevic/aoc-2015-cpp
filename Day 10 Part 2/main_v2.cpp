#include <algorithm>
#include <iostream>
#include <string>

#include <boost/range/irange.hpp>

using uintmax = std::uintmax_t;

auto next_sequence(const std::string& seq) {

  auto result  = std::string{};
  auto count   = uintmax{1};
  auto current = seq.front();

  const auto append = [&] {
    result.push_back('0' + count);
    result.push_back(current);
  };

  for(const auto i : boost::irange({1}, seq.size())) {
    const auto digit = seq[i];
    if(digit == current) {
      ++count;
    } else {
      append();
      count = 1;
      current = digit;
    }
  }
  append();

  return result;
}

auto final_length(const std::string& seq) {

    auto result = uintmax{1};

    std::adjacent_find(seq.begin(), seq.end(), [&result] (const auto a, const auto b) {
      result += (a != b);
      return false;
    });

    return (result * 2);
}

auto solution(std::string seq, uintmax num_iterations) {

  for(const auto i : boost::irange(num_iterations - 1)) {
    seq = next_sequence(seq);
  }

  return final_length(seq);
}

int main() {

    std::cout << solution("1321131112", 50) << std::endl;

}
