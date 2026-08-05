#pragma once

#include <string>

namespace OptiScan::Parser
{
	/** Position in the input stream. */
	struct ScanPosition
	{
		uint64_t _char = 0;
		uint64_t _charInLine = 0;
		uint64_t _line = 0;
		/** */
		ScanPosition() = default;
	};

	/** Single buffered char with position in the input stream. */
	struct ScanChar
	{
		ScanPosition _position;
		char _value = 0;
		/** */
		ScanChar() = default;
	};


}