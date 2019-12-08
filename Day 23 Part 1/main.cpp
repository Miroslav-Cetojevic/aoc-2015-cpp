#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Instruction; // forward declaration

using Register = std::uint64_t;
using Offset = std::int64_t;
using Command = Offset (*) (Register&, const Instruction&);

struct Instruction {
	Command execute;
	std::string register_;
	Offset offset;
};

auto hlf(Register& r, const Instruction& instruction) {
	r /= 2;
	return instruction.offset;
}

auto tpl(Register& r, const Instruction& instruction) {
	r *= 3;
	return instruction.offset;
}

auto inc(Register& r, const Instruction& instruction) {
	++r;
	return instruction.offset;
}

auto jmp(Register&, const Instruction& instruction) {
	return instruction.offset;
}

template<typename P>
auto get_offset(Register r, const Instruction& instruction, P predicate) {
	return (predicate(r) ? instruction.offset : 1);
}

auto jie(Register& r, const Instruction& instruction) {
	return get_offset(r, instruction, [] (auto r) {
		return ((r % 2) == 0);
	});
}

auto jio(Register& r, const Instruction& instruction) {
	return get_offset(r, instruction, [] (auto r) {
		return (r == 1);
	});
}

auto read_stream_jmp(std::stringstream& stream, Instruction& instruction) {
	stream >> instruction.offset;
}

auto read_stream_jix(std::stringstream& stream, Instruction& instruction) {
	stream >> instruction.register_
		   >> instruction.offset;
}

auto read_stream_reg(std::stringstream& stream, Instruction& instruction) {
	stream >> instruction.register_;
	instruction.offset = 1;
}

int main() {

	const auto filename = std::string{"instructions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		const auto strhlf = std::string{"hlf"};
		const auto strtpl = std::string{"tpl"};
		const auto strinc = std::string{"inc"};
		const auto strjmp = std::string{"jmp"};
		const auto strjie = std::string{"jie"};
		const auto strjio = std::string{"jio"};

		auto commands = std::unordered_map<std::string, Command>{
			{strhlf, hlf},
			{strtpl, tpl},
			{strinc, inc},
			{strjmp, jmp},
			{strjie, jie},
			{strjio, jio}
		};

		const auto a = std::string{"a"};
		const auto b = std::string{"b"};

		auto registers = std::unordered_map<std::string, Register>{
			{a, {}},
			{b, {}}
		};

		using Reader = void (*) (std::stringstream&, Instruction&);
		auto stream_readers = std::unordered_map<std::string, Reader>{
			{strhlf, read_stream_reg},
			{strtpl, read_stream_reg},
			{strinc, read_stream_reg},
			{strjmp, read_stream_jmp},
			{strjie, read_stream_jix},
			{strjio, read_stream_jix}
		};

		auto instructions = std::vector<Instruction>{};

		auto stream = std::stringstream{};

		std::string line;
		std::string strcmd;

		while(std::getline(file, line)) {

			stream.str(line);

			stream >> strcmd;

			instructions.push_back({});

			auto& instruction = instructions.back();

			instruction.execute = commands[strcmd];

			// read instructions from stream
			stream_readers[strcmd](stream, instruction);

			stream.clear();
		}

		const auto execute_instruction = [&instructions, &registers] (auto index) {
			const auto& instruction = instructions[index];

			auto& register_ = registers[instruction.register_];

			const auto offset = instruction.execute(register_, instruction);

			return offset;
		};

		// run instructions
		for(decltype(instructions.size()) index = 0; index < instructions.size(); index += execute_instruction(index));

		std::cout << registers[b] << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
