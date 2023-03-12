#pragma once

#include <fstream>
#include <unordered_map>

#include "datatypes.h"
#include "utils.h"
#include "tokenizer.h"

namespace assembler
{
	const std::string INTERMEDIATE_PATH = "intermediate.ime";
	const std::string SYMBOLTABLE_PATH = "symbolTable.sym";
	const std::string OBJECT_PATH = "output.out";

	

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


	


	struct Intermediate
	{
		std::vector<Record> records;
		std::vector<Label> symbolTable;
	};

	void findRecordType(TokenGroup& tokenGroup, RecordType& recordType)
	{
		// variable definition
		if (tokenGroup.tokens[1].type == TokenType::TK_EQUAL)
		{
			// define address variable
			if (tokenGroup.tokens[3].type == TokenType::TK_ADDRESS)
			{
				recordType = RecordType::RT_DEF_ADDRESS;
				return;
			}
			// define literal variable
			if (tokenGroup.tokens[3].type == TokenType::TK_LITERAL)
			{
				recordType = RecordType::RT_DEF_LITERAL;
				return;
			}
		}
		
		// define label
		if (tokenGroup.tokens[1].type == TokenType::TK_COLON)
		{
			recordType = RecordType::RT_DEF_LABEL;
			return;
		}

		// instruction no operand
		if (tokenGroup.tokens[1].type == TokenType::TK_NEWLINE)
		{
			recordType = RecordType::RT_INS_NONE;
			return;
		}

		// instruction with operand
		if (tokenGroup.tokens[1].type == TokenType::TK_COMMA)
		{
			
			// label as operand
			if (tokenGroup.tokens[2].type == TokenType::TK_SYMBOL)
			{
				recordType = RecordType::RT_INS_LABEL;
				return;
			}
			// address as operand
			if (tokenGroup.tokens[3].type == TokenType::TK_ADDRESS)
			{
				recordType = RecordType::RT_INS_ADDRESS;
				return;
			}
			// literal as operand
			if (tokenGroup.tokens[3].type == TokenType::TK_LITERAL)
			{
				recordType = RecordType::RT_INS_LITERAL;
				return;
			}
		}

		utils::Error( utils::ErrorType::ER_INVALID_TOKEN_ORDER, tokenGroup.line );
	}

	void appendLabel(std::vector<Label>& symbolTable, Token symbol, int labelValue, OperandType type, int line)
	{
		for (auto& label : symbolTable)
		{
			if (label.token == symbol)
			{
				utils::Error(utils::ErrorType::ER_MULTIPLY_DEFINED_LABELS, line);
			}
		}
		symbolTable.push_back({ symbol, type, labelValue });
	}

	void firstPass()
	{
		int locationCounter = 0;

		std::ifstream tokenFile(utils::RES_PATH + "tokens.tkz");

		std::ofstream intermediateFile(utils::RES_PATH + INTERMEDIATE_PATH);
		std::ofstream symbolTableFile(utils::RES_PATH + SYMBOLTABLE_PATH);

		std::vector<Label> symbolTable;

		TokenGroup tokenGroup;

		while (tokenFile >> tokenGroup)
		{
			Record record;
			RecordType recordType;

			findRecordType(tokenGroup, recordType);

			switch (recordType)
			{
			case RecordType::RT_DEF_ADDRESS:
				appendLabel(symbolTable, tokenGroup.tokens[0], stoi(tokenGroup.tokens[3].value, nullptr, 16), OperandType::OT_ADDRESS, tokenGroup.line);
				break;
			case RecordType::RT_DEF_LITERAL:
				appendLabel(symbolTable, tokenGroup.tokens[0], stoi(tokenGroup.tokens[3].value, nullptr, 16), OperandType::OT_LITERAL, tokenGroup.line);
				break;
			case RecordType::RT_DEF_LABEL:
				appendLabel(symbolTable, tokenGroup.tokens[0], locationCounter, OperandType::OT_ADDRESS, tokenGroup.line);
				break;

			case RecordType::RT_INS_ADDRESS:
			case RecordType::RT_INS_LITERAL:
			case RecordType::RT_INS_LABEL:
			case RecordType::RT_INS_NONE:
				Operation operation = OpCodeTable[tokenGroup.tokens[0].value];
				//validate operation
				if (operation.mnemonic.empty())
				{
					utils::Error(utils::ErrorType::ER_UNRECOGNIZED_OPERATION, tokenGroup.line);
				}

				locationCounter += operation.wordSize;

				Record record = { recordType, tokenGroup };
				intermediateFile << record;
				break;
			}
		}

		for (auto& label : symbolTable)
		{
			symbolTableFile << label;
		}

		symbolTableFile.close();
		intermediateFile.close();
		tokenFile.close();
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

	void findLabel(std::ifstream& symbolTable, std::string symbol, Label& label, int line)
	{
		// reset stream pointer
		symbolTable.seekg(0);

		// find symbol in symbol table
		Label _label;
		while (symbolTable >> _label)
		{
			if (_label.token.value == symbol)
			{
				label = _label;
				return;
			}
		}
		utils::Error(utils::ErrorType::ER_INVALID_OPERAND, line);
	}

	void validateOperands(OperandType recieved, OperandType expected, int line)
	{
		//validate operands
		if (recieved != expected)
		{
			utils::Error(utils::ErrorType::ER_INVALID_OPERAND, line);
		}
	}

	void assembleInstruction(Operation& operation, Record& record, std::ifstream& symbolTable, std::ofstream& output)
	{
		std::string address;

		switch (record.type)
		{
		case RecordType::RT_INS_ADDRESS:
			
			validateOperands(operation.operandType, OperandType::OT_ADDRESS, record.tokenGroup.line);

			output << operation.opcode;

			unsigned char lowerByte;
			unsigned char upperByte;

			//get address
			address = record.tokenGroup.tokens[3].value;;

			//get address value in hex
			lowerByte = stoi(address.substr(0, 2), nullptr, 16);
			upperByte = stoi(address.substr(2, 2), nullptr, 16);

			output << lowerByte;
			output << upperByte;

			break;
		case RecordType::RT_INS_LITERAL:

			validateOperands(operation.operandType, OperandType::OT_LITERAL, record.tokenGroup.line);

			output << operation.opcode;

			unsigned char literal;

			//get address
			address = record.tokenGroup.tokens[3].value;;

			//get address value in hex
			literal = stoi(address.substr(0, 2), nullptr, 16);

			output << literal;
			break;
		case RecordType::RT_INS_NONE:
			
			validateOperands(operation.operandType, OperandType::OT_NONE, record.tokenGroup.line);

			output << operation.opcode;
			break;
		case RecordType::RT_INS_LABEL:
			Label label;

			findLabel(symbolTable, record.tokenGroup.tokens[2].value, label, record.tokenGroup.line);

			validateOperands(operation.operandType, label.labelType, record.tokenGroup.line);

			output << operation.opcode;

			if (label.labelType == OperandType::OT_ADDRESS)
			{
				//get address
				unsigned char lowerByte = label.labelValue >> 8;
				unsigned char upperByte = label.labelValue;

				output << lowerByte;
				output << upperByte;
				break;
			}

			if (label.labelType == OperandType::OT_LITERAL)
			{
				output << label.labelValue;
				break;
			}
		}
	}

	void secondPass(std::vector<unsigned char>& output)
	{
		std::ifstream intermediateFile(utils::RES_PATH + INTERMEDIATE_PATH);
		std::ifstream symbolTableFile(utils::RES_PATH + SYMBOLTABLE_PATH);
		std::ofstream outputFile(utils::RES_PATH + OBJECT_PATH);

		Record record;
		while (intermediateFile >> record)
		{
			Operation operation = OpCodeTable[record.tokenGroup.tokens[0].value];

			assembleInstruction(operation, record, symbolTableFile, outputFile);
		}

		intermediateFile.close();
		symbolTableFile.close();
		outputFile.close();
	}

	void assemble(std::string filename, std::vector<unsigned char>& output)
	{		
		tokenizer::tokenize(filename);

		assembler::firstPass();

		assembler::secondPass(output);


	}
}