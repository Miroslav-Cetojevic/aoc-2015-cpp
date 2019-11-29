#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

template<typename R, typename T>
auto accumulate(const T& json) -> R {

	auto sum = R{};

	for(const auto& element : json) {

		if(element.is_structured()) {

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
