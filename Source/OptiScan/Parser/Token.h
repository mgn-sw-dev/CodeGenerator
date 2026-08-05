#pragma once

#include <OptiScan/Parser/ScanPosition.h>
#include <string>

namespace OptiScan::Parser
{
	/** Single buffered token with position in the input stream. */
	template<typename TokenKind>
	struct Token
	{
		TokenKind _kind = TokenKind{};
		ScanPosition _position;
		std::string _text;
	};

}
