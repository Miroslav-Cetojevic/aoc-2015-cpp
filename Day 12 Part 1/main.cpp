#include <iostream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

template<typename T, typename N>
void json_foreach(const T& value, N& sum) {
	if(value.IsObject()) {
		for(const auto& element : value.GetObject()) {
			json_foreach(element.value, sum);
		}
	} else if(value.IsArray()) {
		for(const auto& element : value.GetArray()) {
			json_foreach(element, sum);
		}
	} else if(value.IsInt64()) {
		sum += value.GetInt64();
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
		json_foreach(json, sum);

		std::cout << sum << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
