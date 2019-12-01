#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

template<typename O>
auto has_red(const O& object) {

	return std::any_of(object.begin(), object.end(), [] (const auto& element) {

		static const auto blacklisted = std::string{"red"};

		const auto& current_value = element.value;

		const auto result = ((current_value.IsString()) ? current_value.GetString() : std::string{});

		return (result == blacklisted);
	});
}

template<typename N, typename J>
auto accumulate(const J& json) -> N {

	auto sum = N{};

	if(json.IsObject()) {

		auto json_object = json.GetObject();

		if(!has_red(json_object)) {

			for(const auto& element : json_object) {
				sum += accumulate<N>(element.value);
			}
		}

	} else if(json.IsArray()) {

		for(const auto& element : json.GetArray()) {
			sum += accumulate<N>(element);
		}

	} else if(json.IsInt64()) {

		sum += json.GetInt64();
	}

	return sum;
}

int main() {

	auto filename = "json.txt";
	auto file = std::fopen(filename, "r");

	if(file) {

		std::fseek(file, 0, SEEK_END);

		auto json_string = std::string{};
		json_string.resize(std::ftell(file));

		std::rewind(file);

		auto json_stream = rapidjson::FileReadStream{file, json_string.data(), json_string.length()};

		auto json = rapidjson::Document{};

		json.ParseStream(json_stream);

		std::cout << accumulate<std::int64_t>(json) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
