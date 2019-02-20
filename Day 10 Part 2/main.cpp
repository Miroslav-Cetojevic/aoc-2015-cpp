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

	auto range = Range<std::uint64_t>{0, 50};
	std::for_each(range.begin, range.end, [&sequence] (auto) {

		auto tmp = std::string{};
		auto n = std::uint64_t{};

		for(auto j = std::uint64_t{}, pos = (j + 1); j < sequence.size(); j = pos) {
			pos = sequence.find_first_not_of(sequence[j], pos);

			if(pos != sequence.npos) { n = pos-j; }
			else { n = sequence.size()-j; }

			tmp += ('0' + n);
			tmp += sequence[j];
		}

		sequence = tmp;
	});

	std::cout << sequence.size() << std::endl;

	return 0;
}
