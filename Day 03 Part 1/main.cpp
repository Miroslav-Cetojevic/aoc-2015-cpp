#include <complex>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <unordered_map>

using int32 = std::int32_t;
using uint64 = std::uint64_t;

// std::complex gives us the x/y coordinates
// through real()/imag() and also operator+=()
// for free
using Position = std::complex<int32>;

namespace std {
	template<>
	struct hash<Position>{
		auto operator()(const Position& pos) const {
			// encode the position in a 64bit hash
			const auto x = static_cast<uint64>(pos.real());
			const auto y = static_cast<uint64>(pos.imag());
			return x + (y << std::numeric_limits<int32>::digits);
		}
	};
}

template<typename Iterator>
struct Range{
private:
	Iterator m_begin, m_end;
public:
	Range(Iterator first, Iterator last) : m_begin(first), m_end(last) {}
	auto begin() const { return m_begin; }
	auto end() const { return m_end; }
};

int main() {

	const auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		const auto directions = std::unordered_map<char, Position>{
			{'^', {0, 1}},
			{'>', {1, 0}},
			{'v', {0, -1}},
			{'<', {-1, 0}}
		};

		// a map allows us to easily track how many houses have been visited
		auto houses = std::unordered_map<Position, int>{};

		// Santa always starts at pos(0,0)
		auto pos = Position{};

		using iterator = std::istreambuf_iterator<char>;

		const auto begin = iterator{file};
		const auto end   = iterator{};

		for(const auto c : Range{begin, end}) {
			++houses[pos];
			pos += directions.at(c);
		}

		std::cout << houses.size() << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
