#include <fstream>
#include <iostream>

#include "simdjson.h"

int main() {

	const auto filename = "json.txt";

	const auto json_string = simdjson::get_corpus(filename);

	auto json = simdjson::build_parsed_json(json_string);

	if(json.is_valid()) {

		auto json_iterator = simdjson::ParsedJson::Iterator{json};

		auto sum = std::int64_t{};

		do {

			if(json_iterator.is_integer()) {
				sum += json_iterator.get_integer();
			}

		} while(json_iterator.move_forward());

		std::cout << sum << std::endl;

	} else {
		std::cerr << "Error! Could not parse the json file!" << std::endl;
	}

	return 0;
}

