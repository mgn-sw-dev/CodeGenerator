#pragma once

#include <string>

namespace OptiScan::Parser::Dbc
{
	struct DbcComment
	{
		std::string _text;
	};

	struct DbcCommentEnvVar
	{
		std::string _envVarName;
		std::string _text;
	};

	struct DbcCommentMessage
	{
		uint32_t _messageId = 0;
		std::string _text;
	};

	struct DbcCommentNode
	{
		std::string _nodeName;
		std::string _text;
	};

	struct DbcCommentSignal
	{
		uint32_t _messageId = 0;
		std::string _signalName;
		std::string _text;
	};

}
