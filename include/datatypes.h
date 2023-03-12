#pragma once

//struct Label;
//enum class OperandType;
//struct Operation;
//enum class RecordType;
//struct Record;
//enum class TokenType;
//struct Token;
//struct TokenGroup;


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
		return !(*this == other);
	}

	/*friend std::ofstream& operator << (std::ofstream& os, const Token& tk)
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
	}*/
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

	/*friend std::ofstream& operator << (std::ofstream& os, const TokenGroup& tg)
	{
		for (auto& token : tg.tokens)
		{
			os << token;
		}
		os << '/';
		os << tg.line;
		os << '\n';

		return os;
	}*/

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

	void clear()
	{
		tokens.clear();
		line = 0;
	}
};

enum class OperandType
{
	OT_ADDRESS,
	OT_LITERAL,
	OT_NONE,
};

struct Label
{
	Token token;
	OperandType labelType;
	int labelValue;

	friend std::ofstream& operator << (std::ofstream& os, const Label& lb)
	{
		os << lb.token.value;
		os << '|';
		switch (lb.labelType)
		{
		case OperandType::OT_ADDRESS:
			os << 0;
			break;
		case OperandType::OT_LITERAL:
			os << 1;
			break;
		}
		os << lb.labelValue;
		os << '\n';

		return os;
	}

	friend std::ifstream& operator >> (std::ifstream& is, Label& lb)
	{
		char c;
		std::string value;

		//get token
		std::string tokenValue;
		while (is >> c && c != '|')
		{
			tokenValue.push_back(c);
		}

		//get label type
		is >> c;
		OperandType labelType = (c == '0') ? OperandType::OT_ADDRESS : OperandType::OT_LITERAL;

		//get label value
		while (is.get(c) && c != '\n')
		{
			value.push_back(c);
		}
		int labelValue = stoi(value);

		lb = { {TokenType::TK_SYMBOL, tokenValue}, labelType, labelValue };
		return is;
	}
};



struct Operation
{
	std::string mnemonic;
	unsigned char opcode;
	unsigned int wordSize;
	OperandType operandType;
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
	TokenGroup tokenGroup;

	friend std::ofstream& operator << (std::ofstream& os, const Record& rd)
	{
		switch (rd.type)
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
		case RecordType::RT_INS_ADDRESS:
			os << 3;
			break;
		case RecordType::RT_INS_LITERAL:
			os << 4;
			break;
		case RecordType::RT_INS_LABEL:
			os << 5;
			break;
		case RecordType::RT_INS_NONE:
			os << 6;
			break;
		}
		std::string output = "";
		if (rd.tokenGroup.tokens.size() > 1)
		{
			if (rd.tokenGroup.tokens[0].type != TokenType::TK_NEWLINE) // skip newline
			{
				for (auto& token : rd.tokenGroup.tokens)
				{
					switch (token.type)
					{
					case TokenType::TK_SYMBOL:
						output.push_back('!');

						for each (char c in token.value)
						{
							output.push_back(c);
						}

						output.push_back('|');
						break;
					case TokenType::TK_ADDRESS:

						for each (char c in token.value)
						{
							output.push_back(c);
						}

						output.push_back('|');
						break;
					case TokenType::TK_LITERAL:

						for each (char c in token.value)
						{
							output.push_back(c);
						}

						output.push_back('|');
						break;
					case TokenType::TK_PERCENT:
						output.push_back('%');
						break;
					case TokenType::TK_DOLLAR:
						output.push_back('$');
						break;
					case TokenType::TK_EQUAL:
						output.push_back('=');
						break;
					case TokenType::TK_COLON:
						output.push_back(':');
						break;
					case TokenType::TK_COMMA:
						output.push_back(',');
						break;
					}
				}
				output.push_back('/');
				output.append(std::to_string(rd.tokenGroup.line));
				output.push_back('\n');

				os << output;
			}
		}

		return os;
	}

	friend std::ifstream& operator >> (std::ifstream& is, Record& rd)
	{

		RecordType type = RecordType::RT_DEF_ADDRESS;
		TokenGroup tg;

		char c;
		//get record type 
		is >> c;
		switch (c)
		{
		case '0':
			type = RecordType::RT_DEF_ADDRESS;
			break;
		case '1':
			type = RecordType::RT_DEF_LITERAL;
			break;
		case '2':
			type = RecordType::RT_DEF_LABEL;
			break;
		case '3':
			type = RecordType::RT_INS_ADDRESS;
			break;
		case '4':
			type = RecordType::RT_INS_LITERAL;
			break;
		case '5':
			type = RecordType::RT_INS_LABEL;
			break;
		case '6':
			type = RecordType::RT_INS_NONE;
			break;
		}

		//get tokengroup
		is >> tg;

		rd = { type, tg };

		return is;
	}
};




