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

	/*enum class OperationType
	{
		xADDRESS,
		xLITERAL,
		xIO
	};

	std::unordered_map<OperationType, std::vector<std::vector<tokenizer::TokenType>>> OperationTokenTemplate
	{
		{OperationType::xADDRESS, {
			{tokenizer::TokenType::TK_SYMBOL, tokenizer::TokenType::TK_COMMA, tokenizer::TokenType::TK_SYMBOL},
			{tokenizer::TokenType::TK_SYMBOL, tokenizer::TokenType::TK_COMMA, tokenizer::TokenType::TK_DOLLAR, tokenizer::TokenType::TK_ADDRESS}}
		},

		{OperationType::xLITERAL, {
			{tokenizer::TokenType::TK_SYMBOL, tokenizer::TokenType::TK_COMMA, tokenizer::TokenType::TK_SYMBOL},
			{tokenizer::TokenType::TK_SYMBOL, tokenizer::TokenType::TK_COMMA, tokenizer::TokenType::TK_DOLLAR, tokenizer::TokenType::TK_ADDRESS}}
		}
	};*/

	struct Operation
	{
		std::string mnemonic;
		unsigned int opcode;
		unsigned int wordSize;
	};

	std::unordered_map<std::string, Operation> OpCodeTable =
	{
		{"LDA", {"LDA", 0x10, 3}}
	};


	enum class RecordType
	{
		DEF_VAR_ADDRESS,
		DEF_VAR_LITERAL,
		DEF_LABEL,

		INS_ADDRESS,
		INS_LITERAL,
		INS_LABEL,
		INS_NONE
	};

	struct Record
	{
		assembler::RecordType type;
		int locationCounter;
		Operation operation;
		std::vector<tokenizer::Token> tokenGroup;
	};

	utils::Error findRecordType(std::vector<tokenizer::Token>& tokenGroup, assembler::RecordType& recordType)
	{
		
		// variable definition
		if (tokenGroup[1].type == tokenizer::TokenType::TK_EQUAL)
		{
			// define address variable
			if (tokenGroup[3].type == tokenizer::TokenType::TK_ADDRESS)
			{
				recordType = RecordType::DEF_VAR_ADDRESS;
				return { utils::ErrorType::ER_SUCCESS, 0 };
			}
			// define literal variable
			if (tokenGroup[3].type == tokenizer::TokenType::TK_LITERAL)
			{
				recordType = RecordType::DEF_VAR_LITERAL;
				return { utils::ErrorType::ER_SUCCESS, 0 };
			}
		}
		
		// define label
		if (tokenGroup[1].type == tokenizer::TokenType::TK_COLON)
		{
			recordType = RecordType::DEF_LABEL;
			return { utils::ErrorType::ER_SUCCESS, 0 };
		}

		// instruction no operand
		if (tokenGroup[1].type == tokenizer::TokenType::TK_NEWLINE)
		{
			recordType = RecordType::INS_NONE;
			return { utils::ErrorType::ER_SUCCESS, 0 };
		}

		// instruction with operand
		if (tokenGroup[1].type == tokenizer::TokenType::TK_COMMA)
		{
			
			// label as operand
			if (tokenGroup[2].type == tokenizer::TokenType::TK_SYMBOL)
			{
				recordType = RecordType::INS_LABEL;
				return { utils::ErrorType::ER_SUCCESS, 0 };
			}
			// address as operand
			if (tokenGroup[3].type == tokenizer::TokenType::TK_ADDRESS)
			{
				recordType = RecordType::INS_ADDRESS;
				return { utils::ErrorType::ER_SUCCESS, 0 };
			}
			// literal as operand
			if (tokenGroup[3].type == tokenizer::TokenType::TK_LITERAL)
			{
				recordType = RecordType::INS_LITERAL;
				return { utils::ErrorType::ER_SUCCESS, 0};
			}
		}

		return { utils::ErrorType::ER_INVALID_TOKEN_ORDER, tokenGroup[0].line };
	}

	utils::Error assemble(std::vector<std::vector<tokenizer::Token>>& tokens, std::vector<Record>& intermediate)
	{
		std::vector<Record> records;
		for (auto& tokenGroup : tokens)
		{
			RecordType recordType;
			findRecordType(tokenGroup, recordType).assert();

			switch (recordType)
			{
			case RecordType::DEF_VAR_ADDRESS:

				break;
			case RecordType::DEF_VAR_LITERAL:
				break;
			case RecordType::DEF_LABEL:

				break;
			case RecordType::INS_ADDRESS:
				break;
			case RecordType::INS_LITERAL:
				break;
			case RecordType::INS_LABEL:
				break;
			case RecordType::INS_NONE:
				break;
			default:
				break;
			}
		}

		return { utils::ErrorType::ER_SUCCESS, 0 };
	}

	void load(const std::string& filename, std::string& out)
	{

		std::ifstream file(RES_PATH + filename);
		std::stringstream stream;

		if (!file.is_open())
		{
			std::cerr << "unable to open file" << std::endl;
			out = stream.str();
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