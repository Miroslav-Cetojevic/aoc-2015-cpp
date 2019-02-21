#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

int main() {
	auto sequence = std::string{"1321131112"};

	auto range = Range<std::size_t>{0, 50};
	std::for_each(range.begin, range.end, [&sequence] (auto) {

		auto tmp = std::string{};
		auto n = 0UL;

		for(auto i = 0UL, pos = (i + 1); i < sequence.size(); i = pos) {
			pos = sequence.find_first_not_of(sequence[i], pos);

			if(pos != sequence.npos) { n = (pos - i); }
			else { n = (sequence.size() - i); }

			tmp += ('0' + n);
			tmp += sequence[i];
		}

		sequence = tmp;
	});

	std::cout << sequence.size() << std::endl;

	return 0;
}
