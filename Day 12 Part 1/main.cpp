#include <iostream>
#include <string>

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

template<typename T>
auto accumulate(const T& value) -> std::int64_t {
	auto sum = std::int64_t{};

	if(value.IsObject()) {
		for(const auto& element : value.GetObject()) {
			sum += accumulate(element.value);
		}
	} else if(value.IsArray()) {
		for(const auto& element : value.GetArray()) {
			sum += accumulate(element);
		}
	} else if(value.IsInt64()) {
		sum += value.GetInt64();
	}

	return sum;
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

		std::cout << accumulate(json) << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
