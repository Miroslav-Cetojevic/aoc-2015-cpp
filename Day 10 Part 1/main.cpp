#include <iostream>
#include <string>

#include <boost/range/irange.hpp>

using uintmax = std::uintmax_t;

auto solution(std::string seq, uintmax n) {

  const auto size = [&seq] {
    return seq.size();
  };

  for(const auto e : boost::irange(n)) {

    auto tmp = std::string{};

    for(auto i = decltype(e){}, pos = i; i < size(); i = pos) {

      pos = seq.find_first_not_of(seq[i], i + 1);

      if (pos == seq.npos) {
        pos = size();
      }

      tmp.push_back('0' + pos - i);
      tmp.push_back(seq[i]);
    }

    seq = tmp;
  }

  return size();
}

int main() {

    std::cout << solution("1321131112", 40) << std::endl;

    return 0;
}
