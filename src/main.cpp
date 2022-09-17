

#include <string>
#include <vector>
#include "assembler.h"
#include "tokenizer.h"


int main()
{
	std::vector<unsigned char> output;

	std::string filename = "mult.asm";
	//std::string filename = "inc+dec.asm";

	//tokenizer::tokenize(filename);
	assembler::assemble(filename, output);

	return 0;
}
