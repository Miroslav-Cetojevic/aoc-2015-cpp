#include <algorithm>
#include <iostream>
#include <string>
#include <string_view>

bool has_two_pairs(std::string_view pwd) {
  const auto begin = pwd.begin();
  const auto end   = pwd.end();

  const auto first_pair = std::adjacent_find(begin, end);

  return (first_pair < (end - 3)) and (std::adjacent_find((first_pair + 2), end) != end);
}

bool has_incrementing_triple(std::string_view pwd) {
  const auto size = pwd.size();
  auto is_triple = false;
  for(auto i = decltype(size){2}; not is_triple and (i < size); ++i) {
    is_triple = (pwd[i] == (pwd[i-1] + 1)) and (pwd[i-1] == (pwd[i-2] + 1));
  }
  return is_triple;
}

auto is_not_banned (std::string_view pwd) {
  const auto result = std::find_if(pwd.begin(), pwd.end(), [] (const auto c) {
    return (c == 'i' || c == 'o' || c == 'l');
  });
  return (result == pwd.end());
}

auto valid(std::string_view pwd) {
  return has_two_pairs(pwd)
         and has_incrementing_triple(pwd)
         and is_not_banned(pwd);
}

auto increment(std::string& pwd) {
  auto index = (pwd.size() - 1);
  while(pwd[index] == 'z') {
    pwd[index--] = 'a';
  }
  ++pwd[index];
}

auto next_pwd(std::string& pwd) {
  while(not valid(pwd)) {
    increment(pwd);
  }
}

auto solution(std::string pwd) {
  next_pwd(pwd);
  increment(pwd);
  next_pwd(pwd);
  return pwd;
}

int main() {

	const auto pwd = std::string{"cqjxjnds"};

	std::cout << solution(pwd) << std::endl;

	return 0;
}
