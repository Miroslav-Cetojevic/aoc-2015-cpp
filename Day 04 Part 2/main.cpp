/*
 * NOTE: this code uses the poco library and is compiled with the -lPocoFoundation flag
 */
#include <algorithm>
#include <iostream>
#include <string>

#include <boost/iterator/counting_iterator.hpp>

#include <Poco/MD5Engine.h>
#include <Poco/DigestEngine.h>
#include <Poco/DigestStream.h>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

int main() {
	auto key = std::string{"ckczppom"};
	auto prefix = std::string{"000000"};
	auto old_result = 117946UL;

	auto md5 = Poco::MD5Engine{};
	auto out = Poco::DigestOutputStream{md5};

	auto range = Range{(old_result + 1), std::numeric_limits<std::size_t>::max()};
	auto result = std::find_if(range.begin, range.end, [&] (auto i) {
		(out << key + std::to_string(i)).flush();
		return (Poco::DigestEngine::digestToHex(md5.digest()).compare(0, prefix.size(), prefix) == 0);
	});

	std::cout << *result << std::endl;

	return 0;
}
