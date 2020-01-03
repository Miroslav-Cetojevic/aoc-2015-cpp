#include <array>
#include <charconv>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "input.hpp"

using uint16 = std::uint16_t;

using Signal = uint16;
using CarrierID = uint16;
using Wire = std::string_view;

using Wires = std::unordered_map<Wire,CarrierID>;

enum class Gate {
	AND, OR, LSHIFT, RSHIFT, NOT, SIGNAL
};

struct Carrier {
	bool is_named;
	CarrierID id;
};

struct Instruction {
	Gate gate;
	Carrier lhs,rhs;
};
using Instructions = std::vector<Instruction>;

auto split_input(std::string_view input) {
	auto lines = std::vector<decltype(input)>{};
	decltype(input.size()) pos = {};
	while(pos != input.npos) {
		pos = input.find('\n');
		lines.push_back(input.substr({},  pos));
		input.remove_prefix(pos + 1);
	}
	return lines;
}

auto split_line(std::string_view line) {
	auto tokens = std::array<decltype(line), 5>{};
	auto num_tokens = decltype(tokens.size()){};

	const auto delimiter = ' ';
	auto current = decltype(line.size()){};

	while(current != line.npos) {
		const auto end = line.find_first_of(delimiter);
		tokens[num_tokens++] = line.substr({}, end);
		current = line.find_first_not_of(delimiter, end);
		line.remove_prefix(current);
	}
	return std::pair{tokens, num_tokens};
}

auto parse(std::string_view input) {

	auto wires = Wires{};

	auto map_carrier = [&wires, wire_id = CarrierID{}] (Wire wire) mutable {
		auto carrier = Carrier{};
		if(std::isdigit(wire.front())) {
		std::from_chars(wire.begin(),wire.end(),carrier.id);
		} else {
			if(wires.find(wire) == wires.end()) {
				wires[wire] = wire_id++;
			}
			carrier = { true, wires[wire] };
		}
		return carrier;
	};

	const auto parse_instruction = [&] (const auto& tokens, auto size) {
    	auto instruction = Instruction{};

    	auto& gate = instruction.gate;
    	auto& lhs = instruction.lhs;

		switch(size) {
			case 5: {
				// binary operation (and, or, lshift, rshift)
				lhs = map_carrier(tokens[0]);
				instruction.rhs = map_carrier(tokens[2]);
				const auto strgate = tokens[1];
				if(strgate == "AND") {
					gate = Gate::AND;
				} else if(strgate == "OR") {
					gate = Gate::OR;
				} else if(strgate == "LSHIFT") {
					gate = Gate::LSHIFT;
				} else if(strgate == "RSHIFT") {
					gate = Gate::RSHIFT;
				} else {
					std::cerr << "oops, wrong if-else branch in parse_instruction" << std::endl;
				}
			} break;
			case 4: {
				// not operation
				gate = Gate::NOT;
				lhs = map_carrier(tokens[1]);
			} break;
			case 3: {
				// direct signal
				gate = Gate::SIGNAL;
				lhs = map_carrier(tokens[0]);
			} break;
			default: {
				std::cerr << "oops, wrong switch! parse_instruction()" << std::endl;
			}
		}
    	return instruction;
    };

	const auto lines = split_input(input);

	auto instructions = Instructions(lines.size());

	for(const auto line : lines) {
		const auto [tokens, num_tokens] = split_line(line);

		const auto target = map_carrier(tokens[num_tokens-1]);
		instructions[target.id] = parse_instruction(tokens, num_tokens);
	}

	return std::pair{wires,instructions};
}

auto execute(CarrierID target, const Instructions& instructions) {

	const auto size = instructions.size();
	auto visited = std::vector<bool>(size);
	auto signals = std::vector<Signal>(size);

	const auto get_signal = [&](const auto& function, const auto carrier_id) -> Signal {
		const auto lookup = [&](const auto& carrier) {
			const auto id = carrier.id;
			std::decay_t<decltype(id)> result;
			if(carrier.is_named) {
				if(visited[id]) {
					result = signals[id];
				} else {
					result = function(function, id);
				}
			} else {
				result = id;
			}
			return result;
		};

		const auto get_value = [&] {
			const auto& instruction = instructions[carrier_id];
			auto result = lookup(instruction.lhs);
			const auto get_rhs = [&] {
				return lookup(instruction.rhs);
			};
			switch(instruction.gate) {
				case Gate::AND:	   result &= get_rhs();	break;
				case Gate::OR:	   result |= get_rhs();	break;
				case Gate::LSHIFT: result <<= get_rhs();break;
				case Gate::RSHIFT: result >>= get_rhs();break;
				case Gate::NOT:	   result = ~result;	break;
				case Gate::SIGNAL: break;
				default: std::cerr << "oops, wrong switch()! get_value()" << std::endl;
			}
			return result;
		};
		const auto result = get_value();
		visited[carrier_id] = true;
		signals[carrier_id] = result;
		return result;
	};
	return get_signal(get_signal, target);
}

auto solution(std::string_view input) {

	auto [wires, instructions] = parse(input);
	const auto a = wires.at("a");

	const auto part1 = execute(a, instructions);
	std::cout << part1 << std::endl;

	instructions[wires.at("b")] = { Gate::SIGNAL, { {}, part1 }, {} };

	const auto part2 = execute(a, instructions);
	std::cout << part2 << std::endl;
}

int main() {
    solution(puzzle_input);
}
