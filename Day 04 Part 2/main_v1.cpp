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

	using uint32 = std::uint32_t;

	const auto prefix = boost::irange(uint32{3});

	const auto begin = uint32{117946 + 1}; // avoid repeating computations from Part 1
	const auto end   = std::numeric_limits<uint32>::max();

	const auto range = boost::irange(begin, end);

	auto result = std::find_if(range.begin(), range.end(), [&] (auto i) {

		// computes the digest of (key + i) to be retrieved by the engine later
		out << key << i;
		out.close();

		const auto& hash = md5.digest();

		// each element in hash is one-byte-sized (char) and represents two hexadecimal characters -
		// in other words, an all-zero hash[0-2] (3 elements) is equal to "000000"
		const auto has_prefix = std::all_of(prefix.begin(), prefix.end(), [&hash] (auto i) {
			return (hash[i] == 0);
		});

		return has_prefix;
	});

	return *result;
}

int main() {

	std::cout << get_value() << std::endl;

	return 0;
}
