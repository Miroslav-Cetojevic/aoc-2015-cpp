#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using WireID = std::string;
using Signal = std::uint16_t;
using Gate = std::string;

struct Wire {
	WireID id;
	std::optional<Signal> signal;
};

using Inputs = std::vector<Wire>;

struct Operation {
	Wire target;
	Gate gate;
	Inputs inputs;
	WireID prev_target_id;
};

using Operations = std::unordered_map<WireID, Operation>;

using Command = Signal (*) (Signal, Signal);
using Commands = std::unordered_map<std::string, Command>;

auto split(std::vector<std::string>& tokens, const std::string& s, char delimiter) {

   tokens.clear();

   auto token = std::string{};
   auto stream = std::stringstream{s};

   while (std::getline(stream, token, delimiter)) {
      tokens.push_back(token);
   }

   return tokens;
}

auto get_operations_from_file(const std::string& filename) {

	auto operations = Operations{};

	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto make_signal = [] (const auto& id) {
			return (std::isdigit(id.front()) ? std::optional<Signal>(std::stoul(id)) : std::nullopt);
		};

		auto tokens = std::vector<std::string>{};

		auto line = std::string{};

		while(std::getline(file, line)) {

			split(tokens, line, ' ');

			// last element is always a wire id
			auto& wire_id = tokens.back();
			auto& operation = operations[wire_id];
			operation.target.id = wire_id;
			auto& gate = operation.gate;
			auto& inputs = operation.inputs;

			// 4 tokens means a binary operation, 3 tokens is an unary operation,
			// and 2 tokens is a simple assignment
			switch(tokens.size()) {
				case 4: {
					gate = tokens[1];

					auto& lhs_input_id = tokens[0];
					auto& rhs_input_id = tokens[2];

					// that way I can avoid checking any input ids for convertible signals later on
					auto lhs_signal = make_signal(lhs_input_id);
					auto rhs_signal = make_signal(rhs_input_id);

					inputs.push_back({lhs_input_id, lhs_signal});
					inputs.push_back({rhs_input_id, rhs_signal});

					// provided that the input was not convertible to a signal,
					// it has to be the target in another operation
					if(!lhs_signal.has_value()) {
						operations[lhs_input_id].prev_target_id = wire_id;
					}

					if(!rhs_signal.has_value()) {
						operations[rhs_input_id].prev_target_id = wire_id;
					}
				}
					break;
				case 3: {
					gate = tokens[0];

					auto& lhs_input_id = tokens[1];

					auto lhs_signal = make_signal(lhs_input_id);

					inputs.push_back({lhs_input_id, lhs_signal});

					if(!lhs_signal.has_value()) {
						operations[lhs_input_id].prev_target_id = wire_id;
					}
				}
					break;
				default: {
					auto& lhs_input_id = tokens[0];

					auto lhs_signal = make_signal(lhs_input_id);

					inputs.push_back({lhs_input_id, lhs_signal});

					if(!lhs_signal.has_value()) {
						operations[lhs_input_id].prev_target_id = wire_id;
					}
				}
			}

		}

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return operations;
}

auto calculate_target_signal(const WireID& target_id, Operations& operations) {

	auto NOT = [] (const auto lhs, const auto) -> Signal { return ~lhs; };
	auto AND = [] (const auto lhs, const auto rhs) -> Signal { return lhs & rhs; };
	auto OR = [] (const auto lhs, const auto rhs) -> Signal { return lhs | rhs; };
	auto LSHIFT = [] (const auto lhs, const auto rhs) -> Signal { return lhs << rhs; };
	auto RSHIFT = [] (const auto lhs, const auto rhs) -> Signal { return lhs >> rhs; };

	auto commands = Commands{
		{"NOT", NOT},
		{"AND", AND},
		{"OR", OR},
		{"LSHIFT", LSHIFT},
		{"RSHIFT", RSHIFT},
	};

	auto start = operations.find(target_id);
	auto& result = start->second.target.signal;

	for(auto iterator = start; !(result.has_value());) {

		auto& operation = iterator->second;

		auto& target = operation.target;

		auto& target_signal = target.signal;

		// if the target's signal is known, find the operation in which the target
		// is another input, and set the signal of that input
		if(target_signal.has_value()) {

			auto prev_operation = operations.find(operation.prev_target_id);

			auto& inputs = prev_operation->second.inputs;

			auto target_as_input = std::find_if(inputs.begin(), inputs.end(), [&target] (const auto& input) {
				return input.id == target.id;
			});

			target_as_input->signal = target_signal;

			iterator = prev_operation;

		} else {

			auto& inputs = operation.inputs;

			auto all_inputs_have_signals = std::accumulate(inputs.begin(), inputs.end(), true, [&operations] (auto acc, auto& input) {

				// if this input has already a signal as the target in another operation,
				// then this input needs its signal updated
				auto& signal = operations[input.id].target.signal;

				if(signal.has_value()) {
					input.signal = signal;
				}

				return acc && input.signal.has_value();
			});

			if(all_inputs_have_signals) {

				auto& gate = iterator->second.gate;
				auto& lhs_input = inputs.front();

				// no gate means simple assignment to target
				if(gate.empty()) {
					target_signal = lhs_input.signal;
				} else {
					// a gate means there's an operation to do
					auto tmp_signal = Signal{};
					auto lhs_input_signal = lhs_input.signal.value();

					// binary iterator
					if(inputs.size() == 2) {
						auto rhs_input_signal = inputs.back().signal.value();
						tmp_signal = commands[gate](lhs_input_signal, rhs_input_signal);
					} else {
						// unary iterator
						tmp_signal = commands[gate](lhs_input_signal, {});
					}

					target_signal = std::make_optional(tmp_signal);
				}

			} else {

				// get the first input that has no signal and find the operation for which that input is the target
				auto signalless_input = std::find_if_not(inputs.begin(), inputs.end(), [] (const auto& input) {
					return input.signal.has_value();
				});

				iterator = operations.find(signalless_input->id);
			}
		}
	}

	return result.value();
}

int main() {

	auto operations = get_operations_from_file("circuit.txt");

	std::cout << calculate_target_signal("a", operations) << std::endl;

	return 0;
}
