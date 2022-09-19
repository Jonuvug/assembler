
#include <string>
#include "assembler.h"


int main()
{
	std::string filename = "mult.asm";
	//std::string filename = "inc+dec.asm";
	
	assembler::assemble(filename);

	return 0;
}
