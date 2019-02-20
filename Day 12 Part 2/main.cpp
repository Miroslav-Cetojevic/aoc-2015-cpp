#include <algorithm>
#include <fstream>
#include <iostream>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

template<typename O, typename S>
auto has_red(const O& object, const S& blacklisted) {
	return std::any_of(object.begin(), object.end(), [&blacklisted] (auto& element) {
		auto value = ((element.value.IsString()) ? S{element.value.GetString()} : S{});
		return (value == blacklisted);
	});
}

template<typename J, typename S, typename N>
auto json_foreach(const J& json, const S& blacklisted, N& sum) -> void {
	if(json.IsObject()) {
		if(!has_red(json.GetObject(), blacklisted)) {
			for(const auto& element : json.GetObject()) {
				json_foreach(element.value, blacklisted, sum);
			}
		}
	} else if(json.IsArray()) {
		for(const auto& element : json.GetArray()) {
			json_foreach(element, blacklisted, sum);
		}
	} else if(json.IsInt64()) {
		sum += json.GetInt64();
	}
}

int main() {
	auto filename = std::string{"json.txt"};
	auto file = std::fopen(filename.data(), "r");

	if(file) {
		std::fseek(file, 0, SEEK_END);

		auto strjson = std::string{};
		strjson.resize(std::ftell(file));

		std::rewind(file);

		auto stream = rapidjson::FileReadStream{file, strjson.data(), strjson.size()};

		auto json = rapidjson::Document{};
		json.ParseStream(stream);

		auto sum = std::int64_t{};
		auto blacklisted = std::string{"red"};
		json_foreach(json, blacklisted , sum);

		std::cout << sum << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
