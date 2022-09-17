

#include <string>
#include <vector>
#include "assembler.h"
#include "tokenizer.h"


int main()
{
	std::vector<unsigned char> output;
	std::vector<assembler::Label> symbolTable;

	std::string filename = "mult.asm";
	//std::string filename = "inc+dec.asm";

	assembler::firstPass(filename, symbolTable);
	//assembler::assemble(filename, output);

	return 0;
}
