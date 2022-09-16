#pragma once

#include <unordered_map>
#include <iostream>

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
}