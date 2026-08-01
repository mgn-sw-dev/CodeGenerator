#pragma once

#include <cstdint>

namespace OptiScan::Parser::Dbc
{
	class DbcBitTiming
	{
	public:
		uint32_t _baudrate = 0;
		uint32_t _btr1 = 0;
		uint32_t _btr2 = 0;
		/** */
		DbcBitTiming() = default;
		/** */
		bool hasNonZero() const;
	};

}
