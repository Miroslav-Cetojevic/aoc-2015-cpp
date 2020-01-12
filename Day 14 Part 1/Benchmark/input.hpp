#ifndef INPUT_HPP_
#define INPUT_HPP_

#include <string>
#include <string_view>

const std::string miro_input = R"(Vixen can fly 8 km/s for 8 seconds, but then must rest for 53 seconds.
Blitzen can fly 13 km/s for 4 seconds, but then must rest for 49 seconds.
Rudolph can fly 20 km/s for 7 seconds, but then must rest for 132 seconds.
Cupid can fly 12 km/s for 4 seconds, but then must rest for 43 seconds.
Donner can fly 9 km/s for 5 seconds, but then must rest for 38 seconds.
Dasher can fly 10 km/s for 4 seconds, but then must rest for 37 seconds.
Comet can fly 3 km/s for 37 seconds, but then must rest for 76 seconds.
Prancer can fly 9 km/s for 12 seconds, but then must rest for 97 seconds.
Dancer can fly 37 km/s for 1 seconds, but then must rest for 36 seconds.)";

std::string_view seph_input = miro_input;

const std::string rietty_input = R"(Vixen 8 8 53
Blitzen 13 4 49
Rudolph 20 7 132
Cupid 12 4 43
Donner 9 5 38
Dasher 10 4 37
Comet 3 37 76
Prancer 9 12 97
Dancer 37 1 36)";

#endif /* INPUT_HPP_ */
