#pragma once

#include <unordered_map>
#include <iostream>
#include <fstream>

//#include "tokenizer.h"


namespace utils
{
	const std::string RES_PATH = "../../../res/";

	enum class ErrorType
	{
		ER_SUCCESS,

		ER_UNEXPECTED_TOKEN,
		ER_UNRECOGNIZED_CHAR,
		ER_UNRECOGNIZED_NUM,
		ER_INVALID_TOKEN_ORDER,

		ER_LOADING_FILE,

		ER_MULTIPLY_DEFINED_LABELS,
		ER_INVALID_OPERAND,
		ER_UNRECOGNIZED_OPERATION
	};

#pragma warning( push )
#pragma warning( disable : 26495 )
	struct ErrorInfo
	{
		unsigned int errorCode;
		std::string errorMessage;
		bool fatal;
	};
#pragma warning( pop )

	std::unordered_map<ErrorType, ErrorInfo> ErrorInfoMap
	{

		{ ErrorType::ER_UNEXPECTED_TOKEN,			{100,	"\"unexpected token found\"",		true} },
		{ ErrorType::ER_UNRECOGNIZED_CHAR,			{101,	"\"unrecognized character found\"",	true} },
		{ ErrorType::ER_UNRECOGNIZED_NUM,			{102,	"\"unrecognized numerical found\"",	true} },
		{ ErrorType::ER_INVALID_TOKEN_ORDER,		{103,	"\"invalid token order\"",			true} },

		{ ErrorType::ER_LOADING_FILE,				{200,	"\"unable to load file\"",			true} },

		{ ErrorType::ER_MULTIPLY_DEFINED_LABELS,	{301,	"\"multiply defined labels\"",		true} },
		{ ErrorType::ER_INVALID_OPERAND,			{302,	"\"invalid operand type\"",			true} },
		{ ErrorType::ER_UNRECOGNIZED_OPERATION,		{303,	"\"unrecognized operation found\"",	true} }
	};

	struct Error
	{
		ErrorType type;
		bool fatal;
		unsigned int line;

		Error(ErrorType _type, bool _fatal, int _line) :
			type(_type), fatal(_fatal), line(_line)
		{
			if (fatal)
			{
				std::cout << "Error  E" << ErrorInfoMap[type].errorCode << "  " << ErrorInfoMap[type].errorMessage << "  ( line : " << line << " )\n";
				//exit(ErrorInfoMap[type].errorCode);
				exit(-1);
			}
		}

		Error(ErrorType _type, int _line) : Error(_type, ErrorInfoMap[_type].fatal, _line)
		{ }
	};

	enum class FileMode
	{
		FM_RAW_TO_TOKEN,
		FM_TOKEN_TO_INTERMEDIATE,
		FM_INTERMEDIATE_TO_OBJECT,
	};

	class FileManager
	{
	private:

		const std::string INTERMEDIATE_FNAME = "intermediate.ime";
		const std::string SYMBOLTABLE_FNAME = "symbolTable.sym";
		const std::string OBJECT_FNAME = "output.out";
		const std::string TOKEN_FNAME = "tokens.tkz";


		std::ifstream input_stream;
		std::ofstream output_stream;
		FileMode fmode;

		void compileToken(Token& token, std::string& output)
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

		void writeToken(TokenGroup& tokenGroup)
		{
			std::string output = "";
			if (tokenGroup.tokens.size() > 1)
			{
				if (tokenGroup.tokens[0].type != TokenType::TK_NEWLINE) // skip newline
				{
					for (auto& token : tokenGroup.tokens)
					{
						compileToken(token, output);
					}
					output.push_back('/');
					output.append(std::to_string(tokenGroup.line));
					output.push_back('\n');

					output_stream << output;
				}
			}
		}

	public:

		std::istream& next(char& c)
		{
			return input_stream.get(c);
		}

		void write(TokenGroup& tk_group)
		{
			if (fmode != FileMode::FM_RAW_TO_TOKEN)
			{
				// TODO -	throw error : invalid file mode 
				return;
			}
			writeToken(tk_group);
		}

		void close()
		{
			input_stream.close();
			output_stream.close();
		}

		FileManager(const std::string& RAW_FNAME, FileMode transferMode) :
			fmode(transferMode)
		{
			input_stream = std::ifstream(RES_PATH + RAW_FNAME);
			if (!input_stream.is_open())
			{
				utils::Error(utils::ErrorType::ER_LOADING_FILE, 0);
				return;
			}

			output_stream = std::ofstream(utils::RES_PATH + TOKEN_FNAME);
			if (!output_stream.is_open())
			{
				utils::Error(utils::ErrorType::ER_LOADING_FILE, 0);
				return;
			}
		}

		~FileManager()
		{
			close();
		}
	};
}