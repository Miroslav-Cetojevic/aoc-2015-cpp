/*
 * NOTE: this is the same code as Part 1, only the file circuit.txt has changed
 * -> Line 55
 */
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
	std::string operand_lhs;
	std::string operand_rhs;
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
	std::ios_base::sync_with_stdio(false);

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
		const auto max_tokens = 5UL;
		auto tokens = std::vector<std::string>{max_tokens};

		auto line = std::string{};
		while(std::getline(file, line)) {
			boost::split(tokens, line, [](char c) { return (c == ' '); });

			// 4 tokens means a binary operation, 3 tokens is an unary operation,
			// and 2 tokens is a simple assignment
			auto& wire = tokens[tokens.size()-1];
			if(tokens.size() == 4) {
				operations.emplace(wire, Operation{commands[tokens[1]], tokens[0], tokens[2]});
			} else if(tokens.size() == 3) {
				operations.emplace(wire, Operation{commands[tokens[0]], tokens[1], {}});
			} else {
				operations.emplace(wire, Operation{commands[signal], tokens[0], {}});
			}
		}

		/*
		 * execute operations
		 */
		auto get_signal = [] (auto is_operand_signal, auto& operand, auto is_wire_mapped, auto& wire) {
			return (is_operand_signal ? std::stoul(operand) : (is_wire_mapped ? wire->second : 0));
		};

		for(auto operation = operations.begin(); !operations.empty();) {
			auto& strlhs = operation->second.operand_lhs;
			auto& strrhs = operation->second.operand_rhs;

			auto command = operation->second.cmd;

			auto is_lhs_signal = std::isdigit(strlhs.front());
			auto is_rhs_signal = std::isdigit(strrhs.front());

			auto wirelhs = wires.find(strlhs);
			auto wirerhs = wires.find(strrhs);

			auto is_wirelhs_mapped = (wirelhs != wires.end());
			auto is_wirerhs_mapped = (wirerhs != wires.end());

			auto is_lhs_operand_ok = (is_lhs_signal || is_wirelhs_mapped);
			auto is_rhs_operand_ok = (is_rhs_signal || strrhs.empty() || is_wirerhs_mapped);

			// operation has either signals, mapped wires or a mix of both
			if(is_lhs_operand_ok && is_rhs_operand_ok) {
				auto signal_lhs = get_signal(is_lhs_signal, strlhs, is_wirelhs_mapped, wirelhs);
				auto signal_rhs = get_signal(is_rhs_signal, strrhs, is_wirerhs_mapped, wirerhs);

				wires[operation->first] = command(signal_lhs, signal_rhs);

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
