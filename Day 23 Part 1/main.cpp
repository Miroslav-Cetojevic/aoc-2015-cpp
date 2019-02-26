#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct Register {
	std::string name;
	std::size_t value;
};

using Function = bool (*) (Register&);

struct Instruction {
	Function execute;
	std::string register_;
	std::ptrdiff_t offset;
};

auto hlf(Register& r) {
	r.value /= 2;
	return true;
}

auto tpl(Register& r) {
	r.value *= 3;
	return true;
}

auto inc(Register& r) {
	++r.value;
	return true;
}

auto jmp(Register&) { return true; }

auto jie(Register& r) { return ((r.value % 2) == 0); }

auto jio(Register& r) { return (r.value == 1); }


int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"instructions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		auto shlf = std::string{"hlf"};
		auto stpl = std::string{"tpl"};
		auto sinc = std::string{"inc"};
		auto sjmp = std::string{"jmp"};
		auto sjie = std::string{"jie"};
		auto sjio = std::string{"jio"};

		auto functions = std::unordered_map<std::string, Function>{
			{shlf, hlf},
			{stpl, tpl},
			{sinc, inc},
			{sjmp, jmp},
			{sjie, jie},
			{sjio, jio}
		};

		auto a = std::string{"a"};
		auto b = std::string{"b"};

		auto registers = std::unordered_map<std::string, Register> {
			{a, Register{a, 0}},
			{b, Register{b, 0}}
		};

		auto instructions = std::vector<Instruction>{};

		auto ss = std::stringstream{};

		std::string line;
		std::string scmd;
		std::string reg;
		std::ptrdiff_t val;

		while(std::getline(file, line)) {

			ss.str(line);

			ss >> scmd;

			auto cmd = functions[scmd];

			if((scmd == sinc) || (scmd == stpl) || (scmd == shlf)) {
				ss >> reg;
				instructions.emplace_back(Instruction{cmd, reg, 1});
			} else if(scmd == sjmp) {
				ss >> val;
				instructions.emplace_back(Instruction{cmd, {}, val});
			} else {
				// it's either jio or jie
				ss >> reg >> val;
				instructions.emplace_back(Instruction{cmd, reg, val});
			}

			ss.clear();
		}

		auto run_cmd = [&instructions, &registers] (auto index) {
			auto& instruction = instructions[index];

			auto& register_ = registers[instruction.register_];

			auto offset = ((instruction.execute(register_)) ? instruction.offset : 1);

			return offset;
		};

		for(auto index = 0UL; index < instructions.size(); index += run_cmd(index));

		std::cout << registers[b].value << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
