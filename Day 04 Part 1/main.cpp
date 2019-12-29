/*
 * NOTE:
 *
 * This code uses Boost and Poco C++
 *
 * I'm using the following flags:
 * Linker: -lPocoFoundation
 *
 */
#include <algorithm>
#include <iostream>
#include <limits>
#include <string>

#include <boost/range/irange.hpp>

#include <Poco/DigestEngine.h>
#include <Poco/DigestStream.h>
#include <Poco/MD5Engine.h>

auto get_value() {
	const auto key = std::string{"ckczppom"};
	const auto prefix = std::string{"00000"};

	auto md5 = Poco::MD5Engine{};
	auto out = Poco::DigestOutputStream{md5};

	const auto range = boost::irange(std::numeric_limits<std::uint32_t>::max());

	auto result = std::find_if(range.begin(), range.end(), [&] (auto i) {

		// computes the digest of (key + i) to be retrieved by the engine later
		out << (key + std::to_string(i));
		out.close();

		// finishes retrieve the hash in hexadecimal form as a string
		const auto value = Poco::DigestEngine::digestToHex(md5.digest());

		const auto has_prefix = (value.compare(0, prefix.size(), prefix) == 0);

		return has_prefix;
	});

	return *result;
}

int main() {

	std::cout << get_value() << std::endl;

	return 0;
}
