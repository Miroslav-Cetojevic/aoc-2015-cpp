#include <iostream>
#include <sstream>
#include <string>

#include <boost/iterator/counting_iterator.hpp>

template<typename T>
struct Range {
	boost::counting_iterator<T, boost::use_default, T> begin_, end_;
	Range(T b, T e): begin_(b), end_(e) {}
    auto begin() const { return begin_; }
    auto end() const { return end_; }
};
 
int main() {

    auto seq = std::string{"1321131112"};

    for(auto e : Range<unsigned>{0, 40}) {

        auto tmp = std::stringstream{};

        for(auto i = 0UL, pos = (i + 1); i < seq.length(); i = pos, pos = (i + 1)) {

            pos = seq.find_first_not_of(seq[i], pos);

            if (pos == seq.npos) {
                pos = seq.length();
            }

            tmp << (pos - i) << seq[i];

        }

        seq = tmp.str();
    }

    std::cout << seq.length() << std::endl;

    return 0;
}
