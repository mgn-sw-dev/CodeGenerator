#pragma once

#include <OptiScan/Parser/Dbc/DbcSignal.h>
#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
	class DbcMessage
	{
	public:
		static constexpr uint32_t ExtendedCanIdMask = 0x7FFF'FFFF;
		uint32_t _id = 0;
		std::string _name;
		std::vector<DbcSignal> _signals;
		uint32_t _size_byte = 0;
		std::string _transmitter;
		/** */
		DbcMessage() = default;
		/** */
		uint32_t extendedCanId() const;
		/** */
		bool isExtendedCanId() const;
		/** */
		void setExtendedCanId(uint32_t id);
	};

	struct DbcMessageTransmitters
	{
		uint32_t _messageId = 0;
		std::vector<std::string> _transmitters;
	};

}
