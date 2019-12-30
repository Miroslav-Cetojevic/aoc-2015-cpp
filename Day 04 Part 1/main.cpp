/*
 * NOTE:
 *
 * Linker flags required for Poco C++:
 *
 * -lPocoFoundation
 *
 */
#include <algorithm>
#include <iostream>
#include <limits>

#include <boost/range/irange.hpp>

#include <Poco/DigestEngine.h>
#include <Poco/DigestStream.h>
#include <Poco/MD5Engine.h>

auto get_value() {
	const auto key = "ckczppom";

	auto md5 = Poco::MD5Engine{};
	auto out = Poco::DigestOutputStream{md5};

	const auto range = boost::irange(std::numeric_limits<std::uint32_t>::max());

	auto result = std::find_if(range.begin(), range.end(), [&] (auto i) {

		// computes the digest of (key + i)
		out << key << i;
		out.close();

		const auto& hash = md5.digest();

		// each element in hash is one-byte-sized (char) and represents two hexadecimal characters -
		// in other words, the first two elements need to be equal to 0, giving us the first four
		// zero-chars and the third element must be no higher than 0xf ("0f"), so we can get the fifth
		// zero-char.
		const auto has_prefix = (hash[0] == 0) && (hash[1] == 0) && (hash[2] <= 0xf);

		return has_prefix;
	});

	return *result;
}

int main() {

	std::cout << get_value() << std::endl;

	return 0;
}
