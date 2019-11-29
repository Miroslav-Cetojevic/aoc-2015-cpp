#include <fstream>
#include <iostream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

template<typename R, typename T>
auto accumulate(const T& value) -> R {

	auto sum = R{};

	if(value.IsObject()) {

		for(const auto& element : value.GetObject()) {
			sum += accumulate<R>(element.value);
		}

	} else if(value.IsArray()) {

		for(const auto& element : value.GetArray()) {
			sum += accumulate<R>(element);
		}

	} else if(value.IsInt64()) {
		sum += value.GetInt64();
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

		auto json_stream = rapidjson::FileReadStream{file, strjson.data(), strjson.size()};

		auto json_doc = rapidjson::Document{};

		json_doc.ParseStream(json_stream);

		std::cout << accumulate<std::int64_t>(json_doc) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
