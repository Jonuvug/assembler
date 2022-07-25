#pragma once


constexpr auto RES_PATH = "../../../res/";

#include <tokenizer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "utils.h"


namespace assembler
{

	enum class LabelType
	{
		LA_ADDRESS,
		LA_LITERAL,
	};
	
	struct Label
	{
		tokenizer::Token symbol;
		int lcValue;
	};

	struct Operation
	{
		std::string mnemonic;
		unsigned int opcode;
		unsigned int wordSize;
	};

	std::unordered_map<std::string, Operation> OpCodeTable =
	{
		{"HLT", {"HLT", 0x00, 1}},
		{"LDA", {"LDA", 0x10, 3}},
		{"LDI", {"LDI", 0x11, 2}},
		{"ADD", {"ADD", 0x20, 3}},
		{"ADI", {"ADI", 0x21, 2}},
		{"SUB", {"SUB", 0x25, 3}},
		{"SUI", {"SUI", 0x26, 2}},
		{"STA", {"STA", 0x40, 3}},
		{"JMP", {"JMP", 0x50, 3}},
		{"JC",  {"JC",  0x51, 3}},
		{"JZ",  {"JZ",  0x52, 3}},
		{"PRT", {"PRT", 0xE0, 1}},
		{"NOP", {"NOP", 0xFF, 1}},
	};


	enum class RecordType
	{
		RT_DEF_ADDRESS,
		RT_DEF_LITERAL,
		RT_DEF_LABEL,

		RT_INS_ADDRESS,
		RT_INS_LITERAL,
		RT_INS_LABEL,
		RT_INS_NONE
	};

	struct Record
	{
		assembler::RecordType type;
		int locationCounter;
		Operation operation;
		std::vector<tokenizer::Token> tokenGroup;
	};


	struct Intermediate
	{
		std::vector<Record> intermediate;
		std::vector<Label> symbolTable;

	};

	void findRecordType(std::vector<tokenizer::Token>& tokenGroup, assembler::RecordType& recordType)
	{
		
		// variable definition
		if (tokenGroup[1].type == tokenizer::TokenType::TK_EQUAL)
		{
			// define address variable
			if (tokenGroup[3].type == tokenizer::TokenType::TK_ADDRESS)
			{
				recordType = RecordType::RT_DEF_ADDRESS;
				return;
			}
			// define literal variable
			if (tokenGroup[3].type == tokenizer::TokenType::TK_LITERAL)
			{
				recordType = RecordType::RT_DEF_LITERAL;
				return;
			}
		}
		
		// define label
		if (tokenGroup[1].type == tokenizer::TokenType::TK_COLON)
		{
			recordType = RecordType::RT_DEF_LABEL;
			return;
		}

		// instruction no operand
		if (tokenGroup[1].type == tokenizer::TokenType::TK_NEWLINE)
		{
			recordType = RecordType::RT_INS_NONE;
			return;
		}

		// instruction with operand
		if (tokenGroup[1].type == tokenizer::TokenType::TK_COMMA)
		{
			
			// label as operand
			if (tokenGroup[2].type == tokenizer::TokenType::TK_SYMBOL)
			{
				recordType = RecordType::RT_INS_LABEL;
				return;
			}
			// address as operand
			if (tokenGroup[3].type == tokenizer::TokenType::TK_ADDRESS)
			{
				recordType = RecordType::RT_INS_ADDRESS;
				return;
			}
			// literal as operand
			if (tokenGroup[3].type == tokenizer::TokenType::TK_LITERAL)
			{
				recordType = RecordType::RT_INS_LITERAL;
				return;
			}
		}

		utils::Error( utils::ErrorType::ER_INVALID_TOKEN_ORDER, tokenGroup[0].line );
	}

	void appendLabel(std::vector<Label>& symbolTable, tokenizer::Token symbol, int locationCounter)
	{

		for (auto& label : symbolTable)
		{
			if (label.symbol == symbol)
			{
				utils::Error(utils::ErrorType::ER_MULTIPLY_DEFINED_LABELS, symbol.line);

			}
		}
		symbolTable.push_back({ symbol, locationCounter });
	}

	void firstPass(std::vector<std::vector<tokenizer::Token>>& tokens, Intermediate& intermediate)
	{
		std::vector<Record> records;
		int locationCounter = 0;

		for (auto& tokenGroup : tokens)
		{
			RecordType recordType;
			findRecordType(tokenGroup, recordType);
			
			int lineLocationSize = 0;

			switch (recordType)
			{
			case RecordType::RT_DEF_ADDRESS:
				lineLocationSize = 4;
				break;
			case RecordType::RT_DEF_LITERAL:
				lineLocationSize = 2;
				break;
			case RecordType::RT_DEF_LABEL:
				appendLabel(intermediate.symbolTable, tokenGroup[0], locationCounter);
				break;
			case RecordType::RT_INS_ADDRESS:
			case RecordType::RT_INS_LITERAL:
			case RecordType::RT_INS_LABEL:
			case RecordType::RT_INS_NONE:
				lineLocationSize = OpCodeTable[tokenGroup[0].value].wordSize;
				break;
			}

			locationCounter += lineLocationSize;
		}


	}

	void load(const std::string& filename, std::string& out)
	{

		std::ifstream file(RES_PATH + filename);
		std::stringstream stream;

		if (!file.is_open())
		{
			out = stream.str();
			utils::Error( utils::ErrorType::ER_LOADING_FILE, 0 );
			return;
		}

		std::string line;
		while (getline(file, line))
		{
			stream << line << "\n";
		}

		out = stream.str();
	}
}