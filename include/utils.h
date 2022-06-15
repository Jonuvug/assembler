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
		ER_INVALID_TOKEN_ORDER
	};

	struct ErrorInfo
	{
		unsigned int errorCode;
		std::string errorMessage;
		bool success;
	};

	std::unordered_map<ErrorType, ErrorInfo> ErrorInfoMap
	{

		{ ErrorType::ER_SUCCESS,				{0,		"\"successfully compiled\"",		true } },
		{ ErrorType::ER_UNEXPECTED_TOKEN,		{100,	"\"unexpected token found\"",		false} },
		{ ErrorType::ER_UNRECOGNIZED_TOKEN,		{101,	"\"unrecognized token found\"",		false} },
		{ ErrorType::ER_UNRECOGNIZED_CHAR,		{102,	"\"unrecognized character found\"",	false} },
		{ ErrorType::ER_UNRECOGNIZED_NUM,		{103,	"\"unrecognized numerical found\"",	false} },
		{ ErrorType::ER_INVALID_TOKEN_ORDER,	{104,	"\"invalid token order\"",			false} }
	};

	struct Error
	{
		ErrorType type;
		int line;

		bool isSuccess()
		{
			return ErrorInfoMap[type].success;
		}

		void print()
		{
			std::cout << "Error  E" << ErrorInfoMap[type].errorCode << "  " << ErrorInfoMap[type].errorMessage << "  ( line : " << line << " )\n";
		}

		void assert()
		{
			if (!isSuccess())
			{
				print();
				exit(-1);
			}
		}
	};
}