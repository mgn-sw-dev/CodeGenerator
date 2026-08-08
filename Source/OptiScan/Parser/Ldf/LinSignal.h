#pragma once

#include <string>
#include <vector>
#include <variant>

namespace OptiScan::Parser::Ldf
{
	struct LinByteArraySignal
	{
		std::vector<uint8_t> _initValueBigEndian = {};
	};

	struct LinScalarSignal
	{
		uint16_t _initValue = 0;
	};

	using LinSignalValue = std::variant<LinByteArraySignal, LinScalarSignal>;

	struct LinSignal
	{
		uint8_t _bitSize = 0;
		std::string _name;
		std::string _publishedBy;
		std::vector<std::string> _subscribedBy;
		LinSignalValue _value;
		/** */
		LinSignal() = default;
	};
}
