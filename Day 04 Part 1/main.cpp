/*
 * NOTE:
 *
 * This code uses Boost and Poco C++
 *
 * I'm using the following flags:
 * Compiler: -I/usr/include/Poco
 * Linker: -L/usr/lib -lPocoFoundation
 *
 */
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>

#include <boost/iterator/counting_iterator.hpp>

#include <Poco/DigestEngine.h>
#include <Poco/DigestStream.h>
#include <Poco/MD5Engine.h>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin, end;
	Range(T b, T e): begin(b), end(e) {}
};

int main() {
	auto key = std::string{"ckczppom"};
	auto prefix = std::string{"00000"};

	auto md5 = Poco::MD5Engine{};
	auto out = Poco::DigestOutputStream{md5};

	auto range = Range{0U, std::numeric_limits<unsigned>::max()};

	auto result = std::find_if(range.begin, range.end, [&] (auto i) {

		// computes the hash of (key + i) to be retrieved by the engine later
		out << (key + std::to_string(i));
		out.close();

		// retrieve the hash in hexadecimal form as a string
		auto value = Poco::DigestEngine::digestToHex(md5.digest());

		auto has_prefix = (value.compare(0, prefix.size(), prefix) == 0);

		return has_prefix;
	});

	std::cout << *result << std::endl;

	return 0;
}
