#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <fstream>

#include "utils.h"

const std::string TOKEN_PATH = "tokens.tkz";

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
		TK_NEWLINE
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
			return !(*this==other);
		}

		friend std::ofstream& operator << (std::ofstream& os, const Token& tk)
		{
			switch (tk.type)
			{
			case TokenType::TK_SYMBOL:
				os << '!';
				os << tk.value;
				os << '|';
				break;
			case TokenType::TK_ADDRESS:
				os << tk.value;
				os << '|';
				break;
			case TokenType::TK_LITERAL:
				os << tk.value;
				os << '|';
				break;
			case TokenType::TK_PERCENT:
				os << '%';
				break;
			case TokenType::TK_DOLLAR:
				os << '$';
				break;
			case TokenType::TK_EQUAL:
				os << '=';
				break;
			case TokenType::TK_COLON:
				os << ':';
				break;
			case TokenType::TK_COMMA:
				os << ',';
				break;
			}

			return os;
		}
	};

	enum class TokenGroupType
	{
		RT_DEF_ADDRESS,
		RT_DEF_LITERAL,
		RT_DEF_LABEL,

		RT_INS_O1_A,
		RT_INS_O1_L,
		RT_INS_O1_S,
		RT_INS
	};

	struct TokenGroup
	{
		std::vector<Token> tokens;
		int line;

		bool operator == (TokenGroup const& other)
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

		bool operator != (TokenGroup const& other)
		{
			return !(*this == other);
		}

		friend std::ofstream& operator << (std::ofstream& os, const TokenGroup& tg)
		{
			for (auto& token : tg.tokens)
			{
				os << token;
			}
			os << '/';
			os << tg.line;
			os << '\n';

			return os;
		}

		friend std::ifstream& operator >> (std::ifstream& is, TokenGroup& tg)
		{
			std::vector<Token> tokens;
			int line = 0;

			char c;
			TokenType type = TokenType::TK_SYMBOL;
			std::string value;
			std::string symbol;
			bool eol = false;

			while (!eol && !is.eof() && is >> c)
			{
				switch (c)
				{
				case '%':
					// literal
					type = TokenType::TK_LITERAL;
					tokens.push_back({ TokenType::TK_PERCENT, "PERCENT(%)" });
					break;
				case '$':
					// address
					type = TokenType::TK_ADDRESS;
					tokens.push_back({ TokenType::TK_DOLLAR, "DOLLAR($)" });
					break;
				case '=':
					tokens.push_back({ TokenType::TK_EQUAL, "EQUAL(=)" });
					break;
				case ':':
					tokens.push_back({ TokenType::TK_COLON, "COLON(:)" });
					break;
				case ',':
					tokens.push_back({ TokenType::TK_COMMA, "COMMA(,)" });
					break;

				case '|':
					// eof symbol
					tokens.push_back({ type, symbol });
					symbol.clear();
					break;
				case '!':
					// symbol
					type = TokenType::TK_SYMBOL;
					break;

				case '/':
					// end of line
					eol = true;

					//get line number
					value.clear();
					while (is.get(c) && c != '\n')
					{
						value.push_back(c);
					}
					line = stoi(value);

					tokens.push_back({ TokenType::TK_NEWLINE, "NEWLINE(\\n)" });
					break;
				default:
					symbol.push_back(c);
					break;
				}
			}
			tg = { tokens, line };

			return is;
		}
	};

	void appendSymbol(TokenGroup& tokenGroup, std::string& currentSymbol, TokenType& previousTokenType, int currentLine)
	{
		if (currentSymbol.empty())
		{
			return;
		}

		//identify symbol type and verify numericals
		TokenType symbolType;
		switch (previousTokenType)
		{
		case TokenType::TK_DOLLAR:
			// is hex & of length 4
			if (currentSymbol.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentSymbol.length() == 4)
			{
				symbolType = TokenType::TK_ADDRESS;
				break;
			}
			utils::Error(utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine);
			break;
		case TokenType::TK_PERCENT:
			// is hex & of length 2
			if (currentSymbol.find_first_not_of("0123456789abcdefABCDEF") == std::string::npos && currentSymbol.length() == 2)
			{
				symbolType = TokenType::TK_LITERAL;
				break;
			}
			utils::Error(utils::ErrorType::ER_UNRECOGNIZED_NUM, currentLine);
			break;
		default:
			symbolType = TokenType::TK_SYMBOL;
			break;
		}

		tokenGroup.tokens.push_back({ symbolType, currentSymbol });
		currentSymbol.clear();
	}

	void appendToken(TokenGroup& tokenGroup, TokenType type, std::string& currentSymbol, TokenType& previousTokenType, int currentLine, std::string value)
	{
		appendSymbol(tokenGroup, currentSymbol, previousTokenType, currentLine);

		previousTokenType = type;

		tokenGroup.tokens.push_back({ type, value });
	}

	void writeLine(std::ofstream& tokenFile, TokenGroup& tokenGroup, int& currentLine)
	{
		tokenGroup.line = currentLine;

		// skip newlines
		if (tokenGroup.tokens.size() > 1)
		{
			// validate tokens
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
					excpectedTokens = { TokenType::TK_EQUAL, TokenType::TK_COLON, TokenType::TK_PERCENT, TokenType::TK_DOLLAR, TokenType::TK_SYMBOL, TokenType::TK_COMMA, TokenType::TK_NEWLINE };
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

			// write
			tokenFile << tokenGroup;
		}

		currentLine++;
		tokenGroup.tokens.clear();
	}

	void tokenize(const std::string filename)
	{
		// load file
		std::ifstream rawFile(utils::RES_PATH + filename);
		if (!rawFile.is_open())
		{
			utils::Error(utils::ErrorType::ER_LOADING_FILE, 0);
			return;
		}

		int currentLine = 1;
		std::string currentString = "";
		TokenType previousTokenType = TokenType::TK_SYMBOL;
		TokenGroup tokenGroup;

		//open intermediary file
		std::ofstream tokenFile(utils::RES_PATH + TOKEN_PATH);

		char c;
		while(rawFile.get(c))
		{
			switch (c)
			{
			
			case '\t':
				break;

			case ' ':
				appendSymbol(tokenGroup, currentString, previousTokenType, currentLine);
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
				
				writeLine(tokenFile, tokenGroup, currentLine);
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
		appendToken(tokenGroup, TokenType::TK_NEWLINE, currentString, previousTokenType, currentLine, "NEWLINE(\\n)");

		writeLine(tokenFile, tokenGroup, currentLine);

		tokenFile.close();
	}
}