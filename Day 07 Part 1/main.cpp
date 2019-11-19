#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/algorithm/string.hpp>

using Wire = std::string;
using Signal = std::uint16_t;
using Command = Signal (*) (Signal, Signal);

struct Operation {
	Command cmd;
	std::string lhs_operand;
	std::string rhs_operand;
};

template<typename T>
T AND(const T lhs, const T rhs) { return lhs & rhs; }

template<typename T>
T OR(const T lhs, const T rhs) { return lhs | rhs; }

template<typename T>
T LSHIFT(const T lhs, const T rhs) { return lhs << rhs; }

template<typename T>
T RSHIFT(const T lhs, const T rhs) { return lhs >> rhs; }

template<typename T>
T NOT(const T lhs, const T) { return ~lhs; }

template<typename T>
T SIGNAL(const T lhs, const T) { return lhs; }

int main() {

	auto filename = std::string{"circuit.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto signal = std::string{};

		auto commands = std::unordered_map<std::string, Command>{
			{"AND", AND},
			{"OR", OR},
			{"LSHIFT", LSHIFT},
			{"RSHIFT", RSHIFT},
			{"NOT", NOT},
			{signal, SIGNAL}
		};

		auto operations = std::unordered_map<Wire, Operation>{};
		auto wires = std::unordered_map<Wire, Signal>{};

		/*
		 * map each line to a set of operations on a single wire
		 */
		const auto max_tokens = 5U;
		auto tokens = std::vector<std::string>{max_tokens};

		auto line = std::string{};

		while(std::getline(file, line)) {

			boost::split(tokens, line, [] (auto c) { return (c == ' '); });

			// last element is always a wire
			auto& wire = tokens.back();

			// 4 tokens means a binary operation, 3 tokens is an unary operation,
			// and 2 tokens is a simple assignment
			if(tokens.size() == 4) {
				operations.emplace(wire, Operation{commands[tokens[1]], tokens[0], tokens[2]});
			} else if(tokens.size() == 3) {
				operations.emplace(wire, Operation{commands[tokens[0]], tokens[1], {}});
			} else {
				operations.emplace(wire, Operation{commands[signal], tokens[0], {}});
			}
		}

		auto get_signal = [] (auto is_operand_signal, auto& operand, auto is_wire_mapped, auto& wire) {
			return (is_operand_signal ? std::stoul(operand) : (is_wire_mapped ? wire->second : 0));
		};

		//execute operations
		for(auto operation = operations.begin(); !operations.empty();) {

			auto& lhs_str = operation->second.lhs_operand;
			auto& rhs_str = operation->second.rhs_operand;

			auto command = operation->second.cmd;

			auto is_lhs_signal = std::isdigit(lhs_str.front());
			auto is_rhs_signal = std::isdigit(rhs_str.front());

			auto lhs_wire = wires.find(lhs_str);
			auto rhs_wire = wires.find(rhs_str);

			auto is_lhs_wire_mapped = (lhs_wire != wires.end());
			auto is_rhs_wire_mapped = (rhs_wire != wires.end());

			auto is_lhs_operand_ok = (is_lhs_signal || is_lhs_wire_mapped);
			auto is_rhs_operand_ok = (is_rhs_signal || rhs_str.empty() || is_rhs_wire_mapped);

			// operation has either signals, mapped wires or a mix of both
			if(is_lhs_operand_ok && is_rhs_operand_ok) {

				auto lhs_signal = get_signal(is_lhs_signal, lhs_str, is_lhs_wire_mapped, lhs_wire);
				auto rhs_signal = get_signal(is_rhs_signal, rhs_str, is_rhs_wire_mapped, rhs_wire);

				wires[operation->first] = command(lhs_signal, rhs_signal);

				operation = operations.erase(operation);

			} else {
				// no operation possible right now, so move on
				++operation;
			}

			// if there are still operations left at the end
			// of the map, restart the whole process
			if(operation == operations.end() && !operations.empty()) {
				operation = operations.begin();
			}
		}

		std::cout << wires["a"] << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
