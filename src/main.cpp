


#include <string>
#include <vector>
#include "assembler.h"
#include "tokenizer.h"
#include "processor2.h"

int main()
{
	std::string raw;
	assembler::Intermediate intermediate;
	std::vector<std::vector<tokenizer::Token>>tokens;
	std::vector<unsigned char> output;

	assembler::load("mult.asm", raw);
	//assembler::load("inc+dec.asm", raw);
	tokenizer::tokenize(raw, tokens);

	assembler::firstPass(tokens, intermediate);
	assembler::secondPass(intermediate, output);

	processor2::Cpu2 cpu(output);

	cpu.runProgram(0);

	return 0;
}
