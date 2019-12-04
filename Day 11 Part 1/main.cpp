#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

#include <boost/range/adaptor/reversed.hpp>
#include <boost/range/counting_range.hpp>

constexpr auto first_letter = 'a';
constexpr auto last_letter = 'z';

template<typename P>
constexpr auto increment_pwd(P& pwd) {

	for(auto& c : boost::reversed_range{pwd}) {

		if(c != last_letter) {
			++c;
			break;
		}

		c = first_letter;
	}
}

template<typename P>
constexpr auto is_incrementable(const P& pwd) {

	const auto count = std::count_if(pwd.begin(), pwd.end(), [] (const auto letter) {
		return (letter == last_letter);
	});

	const auto size = pwd.size();

	return (static_cast<decltype(size)>(count) != size);
}

template<typename P>
constexpr auto has_two_pairs(const P& pwd) {

	const auto first_result = std::adjacent_find(pwd.begin(), pwd.end());
	const auto has_one_pair = (first_result != pwd.end());

	const auto second_result = std::adjacent_find((first_result + 2), pwd.end());
	const auto has_another_pair = (second_result != pwd.end());

	return has_one_pair && has_another_pair;
}

template<typename P>
constexpr auto has_three_straight(const P& pwd) {

	auto pwdlen = pwd.length();

	const auto range = boost::counting_range<decltype(pwdlen)>(2, pwdlen);

	return std::any_of(range.begin(), range.end(), [&pwd] (auto i) {
		return ((pwd[i]-1) == pwd[i-1]) && ((pwd[i]-2) == pwd[i-2]);
	});
}

template<typename P>
constexpr auto has_banned_letter(const P& pwd) {

	const auto banned_letter = std::find_if(pwd.begin(), pwd.end(), [] (auto c) {
		return c == 'i' || c == 'o' || c == 'l';
	});

	return (banned_letter != pwd.end());
}

template<typename P>
constexpr auto next_pwd(P& pwd) {

	auto not_secure = true;

	while(not_secure && is_incrementable(pwd)) {

		increment_pwd(pwd);

		not_secure = (has_banned_letter(pwd) || !(has_two_pairs(pwd) && has_three_straight(pwd)));

	}
}

int main() {

	auto pwd = std::string{"cqjxjnds"};

	next_pwd(pwd);

	std::cout << pwd << std::endl;

	return 0;
}
