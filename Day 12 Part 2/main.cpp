#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

template<typename O, typename S>
auto has_red(const O& object, const S& blacklisted) {
	return std::any_of(object.begin(), object.end(), [&blacklisted] (auto& element) {
		auto value = ((element.value.IsString()) ? element.value.GetString() : S{});
		return (value == blacklisted);
	});
}

template<typename J, typename S>
auto accumulate(const J& json, const S& blacklisted) -> std::ptrdiff_t {
	auto sum = 0L;

	if(json.IsObject()) {
		if(!has_red(json.GetObject(), blacklisted)) {
			for(const auto& element : json.GetObject()) {
				sum += accumulate(element.value, blacklisted);
			}
		}
	} else if(json.IsArray()) {
		for(const auto& element : json.GetArray()) {
			sum += accumulate(element, blacklisted);
		}
	} else 	if(json.IsInt64()) {
		sum += json.GetInt64();
	}

	return sum;
}

int main() {
	auto filename = "json.txt";
	auto file = std::fopen(filename, "r");

	if(file) {
		std::fseek(file, 0, SEEK_END);

		auto strjson = std::string{};
		strjson.resize(std::ftell(file));

		std::rewind(file);

		auto stream = rapidjson::FileReadStream{file, strjson.data(), strjson.size()};

		auto json = rapidjson::Document{};
		json.ParseStream(stream);

		auto blacklisted = std::string{"red"};

		std::cout << accumulate(json, blacklisted) << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
