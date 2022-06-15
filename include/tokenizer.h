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

		void print()
		{
			std::string out;
			out = value;
			if (type == TokenType::TK_SYMBOL)
				out = "SYMBOL(" + value + ") @ line " + std::to_string(line);
			if (type == TokenType::TK_LITERAL)
				out = "LITERAL(" + value + ") @ line " + std::to_string(line);
			if (type == TokenType::TK_ADDRESS)
				out = "ADDRESS(" + value + ") @ line " + std::to_string(line);

			std::cout << out << "\n";
		}
	};

	

	utils::Error validateTokens(std::vector<std::vector<Token>>& tokens)
	{
		std::vector <TokenType> excpectedTokens = { TokenType::TK_SYMBOL, TokenType::TK_NEWLINE };

		for (auto& tokenGroup : tokens)
		{
			for (auto& token : tokenGroup)
			{
				std::vector<TokenType> possibleTokenTypes;

				switch (token.type)
				{
				case TokenType::TK_SYMBOL:
					possibleTokenTypes = { TokenType::TK_EQUAL, TokenType::TK_COLON, TokenType::TK_COMMA, TokenType::TK_NEWLINE };
					break;
				case TokenType::TK_LITERAL:
					possibleTokenTypes = { TokenType::TK_COMMA, TokenType::TK_NEWLINE };
					break;
				case TokenType::TK_ADDRESS:
					possibleTokenTypes = { TokenType::TK_COMMA, TokenType::TK_NEWLINE };
					break;
				case TokenType::TK_EQUAL:
					possibleTokenTypes = { TokenType::TK_DOLLAR, TokenType::TK_PERCENT };
					break;
				case TokenType::TK_PERCENT:
					possibleTokenTypes = { TokenType::TK_LITERAL };
					break;
				case TokenType::TK_DOLLAR:
					possibleTokenTypes = { TokenType::TK_ADDRESS };
					break;
				case TokenType::TK_COLON:
					possibleTokenTypes = { TokenType::TK_NEWLINE };
					break;
				case TokenType::TK_COMMA:
					possibleTokenTypes = { TokenType::TK_PERCENT, TokenType::TK_DOLLAR, TokenType::TK_SYMBOL };
					break;
				case TokenType::TK_NEWLINE:
					possibleTokenTypes = { TokenType::TK_SYMBOL, TokenType::TK_NEWLINE };
					break;
				default:
					return { utils::ErrorType::ER_UNRECOGNIZED_TOKEN, token.line };
					break;
				}


				if (std::find(excpectedTokens.begin(), excpectedTokens.end(), token.type) == excpectedTokens.end())
				{
					return { utils::ErrorType::ER_UNEXPECTED_TOKEN, token.line };
				}

				excpectedTokens = possibleTokenTypes;
			}
		}

		return { utils::ErrorType::ER_SUCCESS, 0 };
	}

	int stringHexToDecimal(std::string& string)
	{
		int value = 0;
		int pos = 0;
		for each (char c in string)
		{
			int num = 0;
			switch (c)
			{
			case 0:
				num = 0;
				break;
			case 1:
				num = 1;
				break;
			case 2:
				num = 2;
				break;
			case 3:
				num = 3;
				break;
			case 4:
				num = 4;
				break;
			case 5:
				num = 5;
				break;
			case 6:
				num = 6;
				break;
			case 7:
				num = 7;
				break;
			case 8:
				num = 8;
				break;
			case 9:
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



	utils::Error validateString(std::string& currentString, TokenType& previousTokenType, TokenType& stringType, int currentLine)
	{
		// is hex
		if (currentString.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos)
		{
		}

		switch (previousTokenType)
		{
		case TokenType::TK_DOLLAR:
			// is hex & of length 4
			if (currentString.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentString.length() == 4)
			{
				stringType = TokenType::TK_ADDRESS;
				return { utils::ErrorType::ER_SUCCESS, 0 };
			}
			return { utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine };
		case TokenType::TK_PERCENT:
			// is hex & of length 4
			if (currentString.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentString.length() == 2)
			{
				stringType = TokenType::TK_LITERAL;
				return { utils::ErrorType::ER_SUCCESS, 0 };
			}
			return { utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine };

		default:
			stringType = TokenType::TK_SYMBOL;
			return { utils::ErrorType::ER_SUCCESS, 0 };
			break;
		}
		
	}

	utils::Error appendToken(std::vector<Token>& tokens, TokenType type, std::string& currentString, TokenType& previousTokenType, int currentLine, std::string value )
	{
		if (!currentString.empty())
		{
			TokenType stringType;
			validateString(currentString, previousTokenType, stringType, currentLine).assert();
			tokens.push_back({ stringType, currentString, currentLine });
		}

		currentString.clear();
		previousTokenType = type;

		tokens.push_back({ type, value, currentLine });

		return { utils::ErrorType::ER_SUCCESS, 0 };
	}

	utils::Error tokenize(const std::string& string, std::vector<std::vector<Token>>& tokens)
	{
		int currentLine = 1;

		std::string currentString = "";

		TokenType previousTokenType = TokenType::TK_SYMBOL;

		std::vector<Token> tokenGroup;

		utils::Error err = { utils::ErrorType::ER_SUCCESS, 0 };

		for (auto& c : string)
		{
			switch (c)
			{
			case ' ':
			case '\t':
				break;

			case '%':
				err = appendToken(tokenGroup, TokenType::TK_PERCENT, currentString, previousTokenType, currentLine, "PERCENT(%)");
				break;

			case '$':
				err = appendToken(tokenGroup, TokenType::TK_DOLLAR, currentString, previousTokenType, currentLine, "DOLLAR($)");
				break;

			case '=':
				err = appendToken(tokenGroup, TokenType::TK_EQUAL, currentString, previousTokenType, currentLine, "EQUAL(=)");
				break;

			case ':':
				err = appendToken(tokenGroup, TokenType::TK_COLON, currentString, previousTokenType, currentLine, "COLON(:)");
				break;

			case ',':
				err = appendToken(tokenGroup, TokenType::TK_COMMA, currentString, previousTokenType, currentLine, "COMMA(,)");
				break;

			case '\n':
				err = appendToken(tokenGroup, TokenType::TK_NEWLINE, currentString, previousTokenType, currentLine, "NEWLINE(\\n)");
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
				return { utils::ErrorType::ER_UNRECOGNIZED_CHAR, currentLine };
				break;
			}

			if (!err.isSuccess())
			{
				return err;
			}
		}

		return validateTokens(tokens);
	}

	utils::Error tokenize_dev(const std::string& string, std::vector<std::vector<Token>>& tokens)
	{
		utils::Error err = tokenize(string, tokens);

		if (!err.isSuccess())
		{
			err.assert();
		}
		else
		{
			std::cout << "Program successfully compiled" << "\n\n";
			for (auto& tokenGroup : tokens)
			{
				for (auto& token : tokenGroup)
				{
					token.print();
				}
			}
		}

		return err;
	}
}