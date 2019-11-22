#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using Wire = std::string;
using Signal = std::uint16_t;

struct Operation {
	std::string gate;
	std::vector<Wire> operands;
	std::optional<Signal> result;
};

class Operator {

	private:
		using Operations = std::unordered_map<Wire, Operation>;
		Operations operations = {};

		using Command = Signal (*) (Signal, Signal);
		using Commands = std::unordered_map<std::string, Command>;
		Commands commands = {
			{"NOT", [] (const auto lhs, const auto) -> Signal { return ~lhs; }},
			{"AND", [] (const auto lhs, const auto rhs) -> Signal { return lhs & rhs; }},
			{"OR", [] (const auto lhs, const auto rhs) -> Signal { return lhs | rhs; }},
			{"LSHIFT", [] (const auto lhs, const auto rhs) -> Signal { return lhs << rhs; }},
			{"RSHIFT", [] (const auto lhs, const auto rhs) -> Signal { return lhs >> rhs; }}
		};

	public:
		auto add_operation(const Wire& wire, Operation&& operation) {
			operations[wire] = operation;
		}

		auto get_signal(const Wire& wire) -> Signal {

			if(std::isdigit(wire.front())) {
				return std::stoul(wire);
			}

			auto& operation = operations[wire];
			auto& result = operation.result;

			if(result.has_value()) {
				return result.value();
			}

			// there's always a lhs operand
			auto& lhs_operand = operation.operands.front();

			auto new_result = Signal{};

			// no gate means the signal comes from a single source
			if(operation.gate.empty()) {
				new_result = get_signal(lhs_operand);
			} else {
				// a gate and two operands means a binary operation
				if(operation.operands.size() == 2) {
					auto& rhs_operand = operation.operands.back();
					new_result = commands[operation.gate](get_signal(lhs_operand), get_signal(rhs_operand));
				} else {
					// last possibility: there's a gate and it uses only one signal
					new_result = commands[operation.gate](get_signal(lhs_operand), {});
				}
			}

			result = new_result;

			return new_result;
		}
};

// courtesy of Jonathan Boccara, https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
auto split(std::vector<std::string>& tokens, const std::string& s, char delimiter) {

   tokens.clear();

   auto token = std::string{};
   auto stream = std::stringstream{s};

   while (std::getline(stream, token, delimiter)) {
      tokens.push_back(token);
   }

   return tokens;
}

int main() {

	auto filename = std::string{"circuit.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto operator_ = Operator{};

		auto tokens = std::vector<std::string>{};

		auto line = std::string{};

		while(std::getline(file, line)) {

			split(tokens, line, ' ');

			// last element is always a wire
			auto& wire = tokens.back();

			// 4 tokens means a binary operation, 3 tokens is an unary operation,
			// otherwise there are 2 tokens for a simple assignment
			switch(tokens.size()) {
				case 4:
					operator_.add_operation(wire, Operation{tokens[1], {tokens[0], tokens[2]}, {}});
					break;
				case 3:
					operator_.add_operation(wire, Operation{tokens[0], {tokens[1]}, {}});
					break;
				default:
					operator_.add_operation(wire, Operation{{}, {tokens[0]}, {}});
			}
		}

		std::cout << operator_.get_signal("a")<< std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
