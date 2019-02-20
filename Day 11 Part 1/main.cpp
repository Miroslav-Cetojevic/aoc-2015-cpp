#include <algorithm>
#include <functional>
#include <iostream>
#include <string>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

template<typename A, typename C>
constexpr auto is_incremented(const A& alphabet, C& value) {
	auto begin = alphabet.begin();
	auto end = alphabet.end();

	auto pos = std::find_if(begin, end, [&value] (auto c) {
		return (c == value);
	});

	auto has_next = true;
	if(pos == end || ++pos == end) {
		value = *begin;
		has_next = false;
	} else {
		value = *pos;
	}

	return has_next;
}

// string incrementer
template <typename P>
constexpr auto incrementable(P& pwd) {

	auto alphabet = std::string_view{"abcdefghijklmnopqrstuvwxyz"};

	auto count = std::count_if(pwd.begin(), pwd.end(), [&alphabet] (const auto letter) {
		return (letter == alphabet.back());
	});

	auto is_incrementable = (static_cast<std::uint64_t>(count) != pwd.size());

	if(is_incrementable) {
		std::any_of(pwd.rbegin(), pwd.rend(), [&alphabet] (auto& letter) {
			return is_incremented(alphabet, letter);
		});
	}
	return is_incrementable;
}

template<typename P>
constexpr auto has_two_pairs(const P& pwd) {
	auto first_pair = std::adjacent_find(pwd.begin(), pwd.end());
	return (first_pair != pwd.end())
		   && (std::adjacent_find((first_pair + 2), pwd.end()) != pwd.end());
}

template<typename P>
constexpr auto has_three_straight(const P& pwd) {
	auto range = Range<std::uint64_t>{2, pwd.size()};
	return std::any_of(range.begin, range.end, [&pwd] (auto i) {
		return ((pwd[i]-1) == pwd[i-1]) && ((pwd[i]-2) == pwd[i-2]);
	});
}

template<typename P>
constexpr auto next_pwd(P& pwd) {

	while(incrementable(pwd)) {

		auto banned_letter = std::find_if(pwd.begin(), pwd.end(), [] (char c) {
			return c == 'i' || c == 'o' || c == 'l';
		});

		auto has_banned_letter = (banned_letter != pwd.end());

		if(has_banned_letter) { continue; }

		if(has_two_pairs(pwd) && has_three_straight(pwd)) { break; }
	}
}

int main() {
	auto pwd = std::string{"cqjxjnds"};
	next_pwd(pwd);
	std::cout << pwd << std::endl;

	return 0;
}
