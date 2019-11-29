#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
class Range {
	private:
		boost::counting_iterator<T, boost::use_default, T> begin_, end_;
	public:
		Range(T b, T e): begin_(b), end_(e) {}
		auto begin() const { return begin_; }
		auto end() const { return end_; }
};

template<typename T>
class Reverse {
	private:
		T* ptr;
	public:
		Reverse(T& container): ptr(&container) {}
		auto begin() const { return ptr->rbegin(); }
		auto end() const { return ptr->rend(); }
};

template<typename P, typename A>
constexpr auto increment_pwd(P& pwd, const A& alphabet) {

	// todo: for_each(begin(v), find_if(begin(v), end(v), cond), f);?
	for(auto& c : Reverse{pwd}) {

		if(c != alphabet.back()) {
			const auto pos = std::find(alphabet.begin(), alphabet.end(), c);
			c = *pos + 1;
			break;
		}

		c = alphabet.front();
	}
}

template<typename P, typename A>
constexpr auto is_incrementable(const P& pwd, const A& alphabet) {

	const auto count = std::count_if(pwd.begin(), pwd.end(), [&alphabet] (const auto letter) {
		return (letter == alphabet.back());
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

	auto range = Range<decltype(pwdlen)>{2, pwdlen};

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

	const auto alphabet = std::string_view{"abcdefghijklmnopqrstuvwxyz"};

	auto not_secure = true;

	while(not_secure && is_incrementable(pwd, alphabet)) {

		increment_pwd(pwd, alphabet);

		not_secure = (has_banned_letter(pwd) || !(has_two_pairs(pwd) && has_three_straight(pwd)));

	}
}

int main() {

	auto pwd = std::string{"cqjxjnds"};

	next_pwd(pwd);

	std::cout << pwd << std::endl;

	return 0;
}
