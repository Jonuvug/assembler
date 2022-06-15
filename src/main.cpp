


#include <string>
#include <vector>
#include "assembler.h"
#include "tokenizer.h"

int main()
{
	std::string raw;
	std::vector<assembler::Record> intermediate;
	std::vector <std::vector<tokenizer::Token>>tokens;

	assembler::load("mult.asm2", raw);
	tokenizer::tokenize_dev(raw, tokens);

	assembler::assemble(tokens, intermediate);

	return 0;
}
