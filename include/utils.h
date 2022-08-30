#pragma once

#include <unordered_map>

namespace utils
{
	enum class ErrorType
	{
		ER_SUCCESS,

		ER_UNEXPECTED_TOKEN,
		ER_UNRECOGNIZED_TOKEN,
		ER_UNRECOGNIZED_CHAR,
		ER_UNRECOGNIZED_NUM,
		ER_INVALID_TOKEN_ORDER,

		ER_LOADING_FILE,

		ER_MULTIPLY_DEFINED_LABELS
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
		{ ErrorType::ER_UNRECOGNIZED_TOKEN,			{101,	"\"unrecognized token found\"",		true} },
		{ ErrorType::ER_UNRECOGNIZED_CHAR,			{102,	"\"unrecognized character found\"",	true} },
		{ ErrorType::ER_UNRECOGNIZED_NUM,			{103,	"\"unrecognized numerical found\"",	true} },
		{ ErrorType::ER_INVALID_TOKEN_ORDER,		{104,	"\"invalid token order\"",			true} },

		{ ErrorType::ER_LOADING_FILE,				{200,	"\"unable to load file\"",			true} },

		{ ErrorType::ER_MULTIPLY_DEFINED_LABELS,	{301,	"\"multiply defined labels\"",		true} }
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

		Error(ErrorType _type, int _line) : Error(_type, ErrorInfoMap[type].fatal, _line)
		{ }
	};
}