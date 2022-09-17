#pragma once
#pragma warning( push )
#pragma warning( disable : 26495 )
#pragma warning( disable : 6001 )

#include <map>
#include <fstream>

#include "utils.h"


namespace assembler
{
	const std::string INTERMEDIATE_PATH = "intermediate.ime";
	const std::string OBJECT_PATH = "output.out";

	enum class TokenType
	{
		TK_SYMBOL,
		TK_ADDRESS,
		TK_LITERAL,

		TK_PERCENT,
		TK_DOLLAR,
		TK_EQUAL,
		TK_COLON,
		TK_COMMA,
	};

	struct Token
	{
		TokenType type;
		std::string value;

		bool operator == (Token const& other)
		{
			return value == other.value && type == other.type;
		}
		bool operator != (Token const& other)
		{
			return !(*this == other);
		}

		friend std::ofstream& operator << (std::ofstream& os, const Token& tk)
		{
			switch (tk.type)
			{
			case TokenType::TK_SYMBOL:
				os << '!' << tk.value;
				break;
			case TokenType::TK_ADDRESS:
				os << '$' << tk.value;
				break;
			case TokenType::TK_LITERAL:
				os << '%' << tk.value;
				break;
			case TokenType::TK_COMMA:
				os << ',';
				break;
			}

			return os;
		}
	};

	enum class RecordType
	{
		RT_DEF_ADDRESS,
		RT_DEF_LITERAL,
		RT_DEF_LABEL,

		RT_INS_O1_A,
		RT_INS_O1_L,
		RT_INS_O1_S,
		RT_INS
	};

	std::map<std::vector<TokenType>, RecordType> RecordTypeTable =
	{
		{{TokenType::TK_SYMBOL, TokenType::TK_EQUAL, TokenType::TK_DOLLAR, TokenType::TK_ADDRESS},		RecordType::RT_DEF_ADDRESS},
		{{TokenType::TK_SYMBOL, TokenType::TK_EQUAL, TokenType::TK_PERCENT, TokenType::TK_LITERAL},		RecordType::RT_DEF_LITERAL},
		{{TokenType::TK_SYMBOL, TokenType::TK_COLON},													RecordType::RT_DEF_LABEL},
		{{TokenType::TK_SYMBOL, TokenType::TK_DOLLAR, TokenType::TK_ADDRESS},							RecordType::RT_INS_O1_A},
		{{TokenType::TK_SYMBOL, TokenType::TK_PERCENT, TokenType::TK_LITERAL},							RecordType::RT_INS_O1_L},
		{{TokenType::TK_SYMBOL, TokenType::TK_SYMBOL},													RecordType::RT_INS_O1_S},
		{{TokenType::TK_SYMBOL},																		RecordType::RT_INS},
	};

	struct Record
	{
		std::vector<Token> tokens;
		RecordType recordType;
		int line;

		bool operator == (Record const& other)
		{
			if (tokens.size() != other.tokens.size())
			{
				return false;
			}
			for (int i = 0; i < tokens.size(); i++)
			{
				if (tokens[i] != other.tokens[i])
				{
					return false;
				}
			}
			return true;
		}

		bool operator != (Record const& other)
		{
			return !(*this == other);
		}

		friend std::ofstream& operator << (std::ofstream& os, const Record& rd)
		{
			//write tokens
			for (auto& token : rd.tokens)
			{
				os << token;
			}
			os << '/';

			//write record type
			switch (rd.recordType)
			{
			case RecordType::RT_DEF_ADDRESS:
				os << 0;
				break;
			case RecordType::RT_DEF_LITERAL:
				os << 1;
				break;
			case RecordType::RT_DEF_LABEL:
				os << 2;
				break;
			case RecordType::RT_INS_O1_A:
				os << 3;
				break;
			case RecordType::RT_INS_O1_L:
				os << 4;
				break;
			case RecordType::RT_INS_O1_S:
				os << 5;
				break;
			case RecordType::RT_INS:
				os << 6;
				break;
			}

			//write line
			os << rd.line;
			os << '\n';

			return os;
		}

		friend std::ifstream& operator >> (std::ifstream& is, Record& rd)
		{


			std::vector<Token> tokens;
			RecordType recordType;
			int line = 0;

			TokenType type;
			std::string lineNumber;
			std::string symbol;

			char c;
			bool eol = false;

			while (!eol && !is.eof() && is >> c)
			{
				switch (c)
				{
				case '%':
					// literal
					tokens.push_back({ type, symbol });
					symbol.clear();
					type = TokenType::TK_LITERAL;
					break;
				case '$':
					// address
					tokens.push_back({ type, symbol });
					symbol.clear();
					type = TokenType::TK_ADDRESS;
					break;
				case '!':
					// 
					tokens.push_back({ type, symbol });
					symbol.clear();
					type = TokenType::TK_SYMBOL;
					break;

				case ',':
					break;
				case '/':
					tokens.push_back({ type, symbol });

					// end of line
					eol = true;

					//read record type
					is.get(c);
					switch (c)
					{
					case '0':
						recordType = RecordType::RT_DEF_ADDRESS;
						break;
					case '1':
						recordType = RecordType::RT_DEF_LITERAL;
						break;
					case '2':
						recordType = RecordType::RT_DEF_LABEL;
						break;
					case '3':
						recordType = RecordType::RT_INS_O1_A;
						break;
					case '4':
						recordType = RecordType::RT_INS_O1_L;
						break;
					case '5':
						recordType = RecordType::RT_INS_O1_S;
						break;
					case '6':
						recordType = RecordType::RT_INS;
						break;
					default:
						break;
					}

					//read line number
					while (is.get(c) && c != '\n')
					{
						lineNumber.push_back(c);
					}
					line = stoi(lineNumber);
					break;

				default:
					symbol.push_back(c);
					break;
				}
			}

			rd = { tokens, recordType, line };

			return is;
		}
	};

	enum class OperandType
	{
		OT_ADDRESS,
		OT_LITERAL,
		OT_NONE,
	};

	struct Operation
	{
		unsigned char opcode;
		unsigned int wordSize;
		OperandType operandType;
	};

	std::unordered_map<std::string, Operation> OpCodeTable =
	{
		{"HLT", {0x00, 1, OperandType::OT_NONE}},
		{"LDA", {0x10, 3, OperandType::OT_ADDRESS}},
		{"LDI", {0x11, 2, OperandType::OT_LITERAL}},
		{"ADD", {0x20, 3, OperandType::OT_ADDRESS}},
		{"ADI", {0x21, 2, OperandType::OT_LITERAL}},
		{"SUB", {0x25, 3, OperandType::OT_ADDRESS}},
		{"SUI", {0x26, 2, OperandType::OT_LITERAL}},
		{"STA", {0x40, 3, OperandType::OT_ADDRESS}},
		{"JMP", {0x50, 3, OperandType::OT_ADDRESS}},
		{"JC",  {0x51, 3, OperandType::OT_ADDRESS}},
		{"JZ",  {0x52, 3, OperandType::OT_ADDRESS}},
		{"PRT", {0xE0, 1, OperandType::OT_NONE}},
		{"NOP", {0xFF, 1, OperandType::OT_NONE}},
	};

	struct Label
	{
		Token token;
		OperandType labelType;
		int labelValue;
		
	};

	void appendLabel(std::vector<Label>& symbolTable, Token symbol, int labelValue, OperandType type, int currentLine)
	{
		for (auto& label : symbolTable)
		{
			if (label.token == symbol)
			{
				utils::Error(utils::ErrorType::ER_MULTIPLY_DEFINED_LABELS, currentLine);
			}
		}
		symbolTable.push_back({ symbol, type, labelValue });
	}

	void appendSymbol(std::vector<Token>& tokenGroup, std::string& currentSymbol, int currentLine)
	{
		if (currentSymbol.empty())
		{
			return;
		}

		//identify symbol type and verify numericals
		TokenType symbolType = TokenType::TK_SYMBOL;
		if (!tokenGroup.empty())
		{
			switch (tokenGroup.back().type)
			{
			case TokenType::TK_DOLLAR:
				// is hex & of length 4
				if (currentSymbol.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentSymbol.length() == 4)
				{
					symbolType = TokenType::TK_ADDRESS;
					break;
				}
				utils::Error(utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine);
			case TokenType::TK_PERCENT:
				// is hex & of length 2
				if (currentSymbol.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentSymbol.length() == 2)
				{
					symbolType = TokenType::TK_LITERAL;
					break;
				}
				utils::Error(utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine);
			default:
				symbolType = TokenType::TK_SYMBOL;
				break;
			}
		}

		tokenGroup.push_back({ symbolType, currentSymbol });
		currentSymbol.clear();
	}

	void findRecordType(std::vector<Token>& tokenGroup, RecordType& recordType, int currentLine)
	{
		std::vector<TokenType> tokenTypes;
		for (auto& token : tokenGroup)
		{
			tokenTypes.push_back(token.type);
		}
		
		auto search = RecordTypeTable.find(tokenTypes);
		if (search == RecordTypeTable.end())
		{
			// invalid token type order
			utils::Error(utils::ErrorType::ER_INVALID_TOKEN_ORDER, currentLine);
		}

		recordType = search->second;
	}

	void writeLine(std::ofstream& intermediaryFile, std::vector<Token>& tokenGroup, std::vector<Label>& symbolTable, int& locationCounter, int currentLine)
	{
		//skip newline
		if (tokenGroup.size() < 1)
			return;

		RecordType recordType;
		findRecordType(tokenGroup, recordType, currentLine);

		int wordSize = 0;
		Record record;
		switch (recordType)
		{
		case RecordType::RT_DEF_ADDRESS:
			appendLabel(symbolTable, tokenGroup[0], stoi(tokenGroup[3].value, nullptr, 16), OperandType::OT_ADDRESS, currentLine);
			break;
		case RecordType::RT_DEF_LITERAL:
			appendLabel(symbolTable, tokenGroup[0], stoi(tokenGroup[3].value, nullptr, 16), OperandType::OT_LITERAL, currentLine);
			break;
		case RecordType::RT_DEF_LABEL:
			appendLabel(symbolTable, tokenGroup[0], locationCounter, OperandType::OT_ADDRESS, currentLine);
			break;
		case RecordType::RT_INS_O1_A:
			record = { {tokenGroup[0], tokenGroup[2]}, recordType, currentLine };
			wordSize = OpCodeTable[tokenGroup[0].value].wordSize;
			break;
		case RecordType::RT_INS_O1_L:
			record = { {tokenGroup[0], tokenGroup[2]}, recordType, currentLine };
			wordSize = OpCodeTable[tokenGroup[0].value].wordSize;
			break;
		case RecordType::RT_INS_O1_S:
			record = { {tokenGroup[0], tokenGroup[1]}, recordType, currentLine };
			wordSize = OpCodeTable[tokenGroup[0].value].wordSize;
			break;
		case RecordType::RT_INS:
			record = { {tokenGroup[0]}, recordType, currentLine };
			wordSize = OpCodeTable[tokenGroup[0].value].wordSize;
			break;
		}

		locationCounter += wordSize;
		
		tokenGroup.clear();
	}

	void firstPass(const std::string& filename, std::vector<Label>& symbolTable)
	{
		// load file
		std::ifstream rawFile(utils::RES_PATH + filename);
		if (!rawFile.is_open())
			utils::Error(utils::ErrorType::ER_LOADING_FILE, 0);

		//open intermediary file
		std::ofstream intermediaryFile(utils::RES_PATH + INTERMEDIATE_PATH);

		std::vector<Token> tokenGroup;

		int currentLine = 1;
		int locationCounter = 0;
		std::string currentSymbol = "";

		char c;
		while(rawFile.get(c))
		{
			switch (c)
			{
			case '\t':
				break;

			case ' ':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				break;

			case '%':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				tokenGroup.push_back({ TokenType::TK_PERCENT });
				break;

			case '$':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				tokenGroup.push_back({ TokenType::TK_DOLLAR });
				break;

			case '=':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				tokenGroup.push_back({ TokenType::TK_EQUAL });
				break;

			case ':':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				tokenGroup.push_back({ TokenType::TK_COLON });
				break;

			case ',':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				tokenGroup.push_back({ TokenType::TK_COMMA });
				break;

			case '\n':
				appendSymbol(tokenGroup, currentSymbol, currentLine);
				writeLine(intermediaryFile, tokenGroup, symbolTable, locationCounter, currentLine);
				currentLine++;

				break;

			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':

			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':

			case '_':
			case '-':
			case '.':

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				currentSymbol.push_back(c);
				break;

			default:
				utils::Error( utils::ErrorType::ER_UNRECOGNIZED_CHAR, currentLine );
				break;
			}
		}
		appendSymbol(tokenGroup, currentSymbol, currentLine);
		writeLine(intermediaryFile, tokenGroup, symbolTable, locationCounter, currentLine);

		intermediaryFile.close();
	}

	//void assembleInstruction(Operation& operation, Record& record, std::ifstream& symbolTable, std::ofstream& output)
	//{
	//	std::string address;

	//	switch (record.type)
	//	{
	//	case RecordType::RT_INS_ADDRESS:

	//		validateOperands(operation.operandType, OperandType::OT_ADDRESS, record.tokenGroup.line);

	//		output << operation.opcode;

	//		unsigned char lowerByte;
	//		unsigned char upperByte;

	//		//get address
	//		address = record.tokenGroup.tokens[2].value;;

	//		//get address value in hex
	//		lowerByte = stoi(address.substr(0, 2), nullptr, 16);
	//		upperByte = stoi(address.substr(2, 2), nullptr, 16);

	//		output << lowerByte;
	//		output << upperByte;

	//		break;
	//	case RecordType::RT_INS_LITERAL:

	//		validateOperands(operation.operandType, OperandType::OT_LITERAL, record.tokenGroup.line);

	//		output << operation.opcode;

	//		unsigned char literal;

	//		//get address
	//		address = record.tokenGroup.tokens[2].value;;

	//		//get address value in hex
	//		literal = stoi(address.substr(0, 2), nullptr, 16);

	//		output << literal;
	//		break;
	//	case RecordType::RT_INS_NONE:

	//		validateOperands(operation.operandType, OperandType::OT_NONE, record.tokenGroup.line);

	//		output << operation.opcode;
	//		break;
	//	case RecordType::RT_INS_LABEL:
	//		Label label;

	//		findLabel(symbolTable, record.tokenGroup.tokens[1].value, label, record.tokenGroup.line);

	//		validateOperands(operation.operandType, label.labelType, record.tokenGroup.line);

	//		output << operation.opcode;

	//		if (label.labelType == OperandType::OT_ADDRESS)
	//		{
	//			//get address
	//			unsigned char lowerByte = label.labelValue >> 8;
	//			unsigned char upperByte = label.labelValue;

	//			output << lowerByte;
	//			output << upperByte;
	//			break;
	//		}

	//		if (label.labelType == OperandType::OT_LITERAL)
	//		{
	//			output << label.labelValue;
	//			break;
	//		}
	//	}
	//}

	//void secondPass(std::vector<unsigned char>& output)
	//{
	//	std::ifstream intermediateFile(utils::RES_PATH + INTERMEDIATE_PATH);
	//	std::ofstream outputFile(utils::RES_PATH + OBJECT_PATH);

	//	Record record;
	//	while (intermediateFile >> record)
	//	{
	//		Operation operation = OpCodeTable[record.tokens[0].value];

	//		assembleInstruction(operation, record, symbolTableFile, outputFile);
	//	}

	//	intermediateFile.close();
	//	outputFile.close();
	//}
}

#pragma warning( pop )