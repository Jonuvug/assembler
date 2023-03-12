#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>

#include "datatypes.h"
#include "utils.h"

const std::string TOKEN_FNAME = "tokens.tkz";

namespace tokenizer
{
	
	void validateTokens(TokenGroup& tokenGroup)
	{

		std::vector <TokenType> excpectedTokens = { TokenType::TK_SYMBOL, TokenType::TK_NEWLINE };

		for (auto& token : tokenGroup.tokens)
		{
			if (std::find(excpectedTokens.begin(), excpectedTokens.end(), token.type) == excpectedTokens.end())
			{
				utils::Error(utils::ErrorType::ER_UNEXPECTED_TOKEN, tokenGroup.line);
			}
			switch (token.type)
			{
			case TokenType::TK_SYMBOL:
				excpectedTokens = { TokenType::TK_EQUAL, TokenType::TK_COLON, TokenType::TK_COMMA, TokenType::TK_NEWLINE };
				break;
			case TokenType::TK_LITERAL:
				excpectedTokens = { TokenType::TK_COMMA, TokenType::TK_NEWLINE };
				break;
			case TokenType::TK_ADDRESS:
				excpectedTokens = { TokenType::TK_COMMA, TokenType::TK_NEWLINE };
				break;
			case TokenType::TK_EQUAL:
				excpectedTokens = { TokenType::TK_DOLLAR, TokenType::TK_PERCENT };
				break;
			case TokenType::TK_PERCENT:
				excpectedTokens = { TokenType::TK_LITERAL };
				break;
			case TokenType::TK_DOLLAR:
				excpectedTokens = { TokenType::TK_ADDRESS };
				break;
			case TokenType::TK_COLON:
				excpectedTokens = { TokenType::TK_NEWLINE };
				break;
			case TokenType::TK_COMMA:
				excpectedTokens = { TokenType::TK_PERCENT, TokenType::TK_DOLLAR, TokenType::TK_SYMBOL };
				break;
			}
		}
	}

	void identifySymbol(std::string& currentSymbol, TokenType& previousTokenType, TokenType& stringType, int currentLine)
	{
		switch (previousTokenType)
		{
		case TokenType::TK_DOLLAR:
			// is hex & of length 4
			if (currentSymbol.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentSymbol.length() == 4)
			{
				stringType = TokenType::TK_ADDRESS;
				break;
			}
			utils::Error( utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine );
			break;
		case TokenType::TK_PERCENT:
			// is hex & of length 2
			if (currentSymbol.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentSymbol.length() == 2)
			{
				stringType = TokenType::TK_LITERAL;
				break;
			}
			utils::Error( utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine );
			break;
		default:
			stringType = TokenType::TK_SYMBOL;
			break;
		}
	}

	void appendToken(TokenGroup& tokenGroup, TokenType type, std::string& currentSymbol, TokenType& previousTokenType, int currentLine, std::string value)
	{
		if (!currentSymbol.empty())
		{
			TokenType symbolType;

			identifySymbol(currentSymbol, previousTokenType, symbolType, currentLine);

			tokenGroup.tokens.push_back({ symbolType, currentSymbol });
		}

		currentSymbol.clear();
		previousTokenType = type;

		tokenGroup.tokens.push_back({ type, value });
	}

	//void writeLine(std::ofstream& tokenFile, TokenGroup& tokenGroup, int& currentLine)
	//{
	//	tokenGroup.line = currentLine;

	//	// skip newlines
	//	if (tokenGroup.tokens.size() > 1)
	//	{
	//		// validate and write onto tokenfile
	//		validateTokens(tokenGroup);
	//		tokenFile << tokenGroup;

	//	}

	//	currentLine++;
	//	tokenGroup.tokens.clear();
	//}

	void tokenize(const std::string filename)
	{
		// load file
		/*std::ifstream rawFile(utils::RES_PATH + filename);
		if (!rawFile.is_open())
		{
			utils::Error(utils::ErrorType::ER_LOADING_FILE, 0);
			return;
		}*/

		int currentLine = 1;
		std::string currentSymbol = "";
		TokenType previousTokenType = TokenType::TK_SYMBOL;
		TokenGroup tokenGroup;

		//open intermediary file
		//std::ofstream tokenFile(utils::RES_PATH + TOKEN_FNAME);

		utils::FileManager fm(filename, utils::FileMode::FM_RAW_TO_TOKEN);

		char c;
		while(fm.next(c))
		{
			switch (c)
			{
			case ' ':
			case '\t':
				break;

			case '%':
				appendToken(tokenGroup, TokenType::TK_PERCENT, currentSymbol, previousTokenType, currentLine, "PERCENT(%)");
				break;

			case '$':
				appendToken(tokenGroup, TokenType::TK_DOLLAR, currentSymbol, previousTokenType, currentLine, "DOLLAR($)");
				break;

			case '=':
				appendToken(tokenGroup, TokenType::TK_EQUAL, currentSymbol, previousTokenType, currentLine, "EQUAL(=)");
				break;

			case ':':
				appendToken(tokenGroup, TokenType::TK_COLON, currentSymbol, previousTokenType, currentLine, "COLON(:)");
				break;

			case ',':
				appendToken(tokenGroup, TokenType::TK_COMMA, currentSymbol, previousTokenType, currentLine, "COMMA(,)");
				break;

			case '\n':
				appendToken(tokenGroup, TokenType::TK_NEWLINE, currentSymbol, previousTokenType, currentLine, "NEWLINE(\\n)");
				
				tokenGroup.line = currentLine;

				validateTokens(tokenGroup);
				
				fm.write(tokenGroup);
				currentLine++;

				tokenGroup.clear();

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
		appendToken(tokenGroup, TokenType::TK_NEWLINE, currentSymbol, previousTokenType, currentLine, "NEWLINE(\\n)");

		tokenGroup.line = currentLine;

		validateTokens(tokenGroup);

		fm.write(tokenGroup);
	}
}