#pragma once


#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include "utils.h"

namespace tokenizer
{
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
		TK_NEWLINE,
	};

	struct Token
	{
		TokenType type;
		std::string value;
		int line;

		bool operator == (Token const& other)
		{
			return value == other.value;
		}
	};

	void printTokens(std::vector<std::vector<Token>>& tokens)
	{
		std::string out;
		for (auto& tokenGroup : tokens)
		{
			for (auto& token : tokenGroup)
			{
				
				out = token.value;
				if (token.type == TokenType::TK_SYMBOL)
					out = "SYMBOL(" + token.value + ") @ line " + std::to_string(token.line);
				if (token.type == TokenType::TK_LITERAL)
					out = "LITERAL(" + token.value + ") @ line " + std::to_string(token.line);
				if (token.type == TokenType::TK_ADDRESS)
					out = "ADDRESS(" + token.value + ") @ line " + std::to_string(token.line);
				out += '\n';
			}
		}
		std::cout << out << "\n";
	}

	void validateTokens(std::vector<std::vector<Token>>& tokens)
	{
		std::vector <TokenType> excpectedTokens = { TokenType::TK_SYMBOL, TokenType::TK_NEWLINE };

		for (auto& tokenGroup : tokens)
		{
			for (auto& token : tokenGroup)
			{
				if (std::find(excpectedTokens.begin(), excpectedTokens.end(), token.type) == excpectedTokens.end())
				{
					utils::Error( utils::ErrorType::ER_UNEXPECTED_TOKEN, token.line );
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
				case TokenType::TK_NEWLINE:
					excpectedTokens = { TokenType::TK_SYMBOL, TokenType::TK_NEWLINE };
					break;
				default:
					utils::Error( utils::ErrorType::ER_UNRECOGNIZED_TOKEN, token.line );
				}
			}
		}
	}

	void validateString(std::string& currentString, TokenType& previousTokenType, TokenType& stringType, int currentLine)
	{
		switch (previousTokenType)
		{
		case TokenType::TK_DOLLAR:
			// is hex & of length 4
			if (currentString.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentString.length() == 4)
			{
				stringType = TokenType::TK_ADDRESS;
				break;
			}
			utils::Error( utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine );
			break;
		case TokenType::TK_PERCENT:
			// is hex & of length 4
			if (currentString.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentString.length() == 2)
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

	void appendToken(std::vector<Token>& tokenGroup, TokenType type, std::string& currentString, TokenType& previousTokenType, int currentLine, std::string value)
	{
		if (!currentString.empty())
		{
			TokenType stringType;
			validateString(currentString, previousTokenType, stringType, currentLine);
			tokenGroup.push_back({ stringType, currentString, currentLine });
		}

		currentString.clear();
		previousTokenType = type;

		tokenGroup.push_back({ type, value, currentLine });
	}

	void tokenize(const std::string& rawData, std::vector<std::vector<Token>>& tokens)
	{
		int currentLine = 1;

		std::string currentString = "";

		TokenType previousTokenType = TokenType::TK_SYMBOL;

		std::vector<Token> tokenGroup;

		for (auto& c : rawData)
		{
			switch (c)
			{
			case ' ':
			case '\t':
				break;

			case '%':
				appendToken(tokenGroup, TokenType::TK_PERCENT, currentString, previousTokenType, currentLine, "PERCENT(%)");
				break;

			case '$':
				appendToken(tokenGroup, TokenType::TK_DOLLAR, currentString, previousTokenType, currentLine, "DOLLAR($)");
				break;

			case '=':
				appendToken(tokenGroup, TokenType::TK_EQUAL, currentString, previousTokenType, currentLine, "EQUAL(=)");
				break;

			case ':':
				appendToken(tokenGroup, TokenType::TK_COLON, currentString, previousTokenType, currentLine, "COLON(:)");
				break;

			case ',':
				appendToken(tokenGroup, TokenType::TK_COMMA, currentString, previousTokenType, currentLine, "COMMA(,)");
				break;

			case '\n':
				appendToken(tokenGroup, TokenType::TK_NEWLINE, currentString, previousTokenType, currentLine, "NEWLINE(\\n)");
				currentLine++;
				// not only newline
				if (tokenGroup.size() > 1)
				{
					tokens.push_back(tokenGroup);
				}
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
				currentString.push_back(c);
				break;

			default:
				utils::Error( utils::ErrorType::ER_UNRECOGNIZED_CHAR, currentLine );
				break;
			}
		}

		validateTokens(tokens);
	}
}