


#include <string>
#include <vector>
#include "assembler.h"
#include "tokenizer.h"

int main()
{
	std::string raw;
	assembler::Intermediate intermediate;
	std::vector <std::vector<tokenizer::Token>>tokens;

	assembler::load("mult.asm2", raw);
	tokenizer::tokenize(raw, tokens);

	assembler::firstPass(tokens, intermediate);

	return 0;
}
