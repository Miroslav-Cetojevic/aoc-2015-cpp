#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

template<typename O>
auto has_red(const O& object) {

	return std::any_of(object.begin(), object.end(), [] (const auto& element) {

		static const auto blacklisted = std::string{"red"};

		const auto value = (element.is_string() ? element.template get<std::string>() : std::string{});

		return (value == blacklisted);
	});
}


template<typename R, typename T>
auto accumulate(const T& json) -> R {

	auto sum = R{};

	for(const auto& element : json) {

		if((element.is_object() && !has_red(element)) || element.is_array()) {

			sum += accumulate<R>(element);

		} else if(element.is_number()) {

			sum += element.template get<R>();

		}
	}

	return sum;
}

int main() {

	auto filename = std::string{"json.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto json = nlohmann::json{};

		file >> json;

		std::cout << accumulate<std::int64_t>(json) << std::endl;

	} else {
		std::cerr << "Error! Could not open " << filename << "!" << std::endl;
	}

	return 0;
}
