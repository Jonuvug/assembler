#pragma once
#pragma warning( disable : 26495 )

constexpr auto RES_PATH = "../../../res/";

#include <tokenizer.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>

#include "utils.h"


namespace assembler
{

	enum class OperandType
	{
		OT_ADDRESS,
		OT_LITERAL,
		OT_NONE,
	};
	
	struct Label
	{
		tokenizer::Token token;
		OperandType labelType;
		int labelValue;
	};


	struct Operation
	{
		std::string mnemonic;
		unsigned char opcode;
		unsigned int wordSize;
		OperandType operandType;
	};

	std::unordered_map<std::string, Operation> OpCodeTable =
	{
		{"HLT", {"HLT", 0x00, 1, OperandType::OT_NONE}},
		{"LDA", {"LDA", 0x10, 3, OperandType::OT_ADDRESS}},
		{"LDI", {"LDI", 0x11, 2, OperandType::OT_LITERAL}},
		{"ADD", {"ADD", 0x20, 3, OperandType::OT_ADDRESS}},
		{"ADI", {"ADI", 0x21, 2, OperandType::OT_LITERAL}},
		{"SUB", {"SUB", 0x25, 3, OperandType::OT_ADDRESS}},
		{"SUI", {"SUI", 0x26, 2, OperandType::OT_LITERAL}},
		{"STA", {"STA", 0x40, 3, OperandType::OT_ADDRESS}},
		{"JMP", {"JMP", 0x50, 3, OperandType::OT_ADDRESS}},
		{"JC",  {"JC",  0x51, 3, OperandType::OT_ADDRESS}},
		{"JZ",  {"JZ",  0x52, 3, OperandType::OT_ADDRESS}},
		{"PRT", {"PRT", 0xE0, 1, OperandType::OT_NONE}},
		{"NOP", {"NOP", 0xFF, 1, OperandType::OT_NONE}},
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
		RecordType type;
		int locationCounter;
		std::vector<tokenizer::Token> tokenGroup;
	};


	struct Intermediate
	{
		std::vector<Record> records;
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

	void appendLabel(std::vector<Label>& symbolTable, tokenizer::Token symbol, int labelValue, OperandType type)
	{

		for (auto& label : symbolTable)
		{
			if (label.token == symbol)
			{
				utils::Error(utils::ErrorType::ER_MULTIPLY_DEFINED_LABELS, symbol.line);
			}
		}
		symbolTable.push_back({ symbol, type, labelValue });
	}

	void firstPass(std::vector<std::vector<tokenizer::Token>>& tokens, Intermediate& intermediate)
	{
		int locationCounter = 0;

		for (auto& tokenGroup : tokens)
		{
			Record record;
			RecordType recordType;
			unsigned int instructionWordSize = 0;

			findRecordType(tokenGroup, recordType);

			switch (recordType)
			{
			case RecordType::RT_DEF_ADDRESS:
				appendLabel(intermediate.symbolTable, tokenGroup[0], stoi(tokenGroup[3].value, nullptr, 16), OperandType::OT_ADDRESS);
				break;
			case RecordType::RT_DEF_LITERAL:
				appendLabel(intermediate.symbolTable, tokenGroup[0], stoi(tokenGroup[3].value, nullptr, 16), OperandType::OT_LITERAL);
				break;
			case RecordType::RT_DEF_LABEL:
				appendLabel(intermediate.symbolTable, tokenGroup[0], locationCounter, OperandType::OT_ADDRESS);
				break;
			case RecordType::RT_INS_ADDRESS:
			case RecordType::RT_INS_LITERAL:
			case RecordType::RT_INS_LABEL:
			case RecordType::RT_INS_NONE:
				Operation operation = OpCodeTable[tokenGroup[0].value];
				//validate operation
				if (operation.mnemonic.empty())
				{
					utils::Error(utils::ErrorType::ER_UNRECOGNIZED_OPERATION, tokenGroup[0].line);
				}
				instructionWordSize = operation.wordSize;
				break;
			}

			locationCounter += instructionWordSize;
			intermediate.records.push_back({ recordType, locationCounter, tokenGroup });
		}
	}

	int stringHexToDecimal(std::string string)
	{
		

		int value = 0;
		int pos = 0;
		for each (char c in string)
		{
			int num;
			switch (c)
			{
			case '0':
				num = 0;
				break;
			case '1':
				num = 1;
				break;
			case '2':
				num = 2;
				break;
			case '3':
				num = 3;
				break;
			case '4':
				num = 4;
				break;
			case '5':
				num = 5;
				break;
			case '6':
				num = 6;
				break;
			case '7':
				num = 7;
				break;
			case '8':
				num = 8;
				break;
			case '9':
				num = 9;
				break;
			default:
				num = 0;
				break;
			}
			value += num * static_cast<int>(pow(16, pos));
			pos++;
		}
		return value;
	}

	void findLabel(std::vector<Label>& symbolTable, std::string symbol, Label& label)
	{
		// find symbol in symbol table
		for (auto& _label : symbolTable)
		{
			if (_label.token.value == symbol)
			{
				label = _label;
			}
		}
	}

	void secondPass(Intermediate& intermediate, std::vector<unsigned char>& output)
	{
		unsigned int byteIndex = 0;

		for (auto& record : intermediate.records)
		{
			Operation operation = OpCodeTable[record.tokenGroup[0].value];
			switch (record.type)
			{
			case RecordType::RT_INS_ADDRESS:
				//validate operands
				if (operation.operandType != OperandType::OT_ADDRESS)
				{
					utils::Error(utils::ErrorType::ER_INVALID_OPERAND, record.tokenGroup[0].line);
				}

				output.push_back(operation.opcode);

				unsigned char lowerByte;
				unsigned char upperByte;

				//get address value in hex
				lowerByte = stoi(record.tokenGroup[3].value.substr(0, 2), nullptr, 16);
				upperByte = stoi(record.tokenGroup[3].value.substr(2, 2), nullptr, 16);

				output.push_back(lowerByte);
				output.push_back(upperByte);
				break;
			case RecordType::RT_INS_LITERAL:
				//validate operands
				if (operation.operandType != OperandType::OT_LITERAL)
				{
					utils::Error(utils::ErrorType::ER_INVALID_OPERAND, record.tokenGroup[0].line);
				}

				output.push_back(operation.opcode);

				unsigned char literal;

				//get address value in hex
				literal = stoi(record.tokenGroup[3].value.substr(0, 2), nullptr, 16);

				output.push_back(literal);
				break;
			case RecordType::RT_INS_NONE:
				//validate operands
				if (operation.operandType != OperandType::OT_NONE)
				{
					utils::Error(utils::ErrorType::ER_INVALID_OPERAND, record.tokenGroup[0].line);
				}

				output.push_back(operation.opcode);
				break;
			case RecordType::RT_INS_LABEL:
				Label label;
				
				findLabel(intermediate.symbolTable, record.tokenGroup[2].value, label);

				//validate operands
				if (operation.operandType != label.labelType)
				{
					utils::Error(utils::ErrorType::ER_INVALID_OPERAND, record.tokenGroup[0].line);
				}

				output.push_back(operation.opcode);

				if (label.labelType == OperandType::OT_ADDRESS)
				{
					//get address
					unsigned char lowerByte = label.labelValue >> 8;
					unsigned char upperByte = label.labelValue;

					output.push_back(lowerByte);
					output.push_back(upperByte);
					break;
				}

				if (label.labelType == OperandType::OT_LITERAL)
				{
					output.push_back(label.labelValue);
					break;
				}
			}
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