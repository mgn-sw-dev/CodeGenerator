#pragma once

#include <string>
#include <vector>
#include <variant>

namespace OptiScan::Parser::Ldf
{
	struct LinAsciiValue
	{
	};

	struct LinBcdValue
	{
	};

	struct LinLogicalValue
	{
		uint32_t _signalValue = 0;
		std::string _textInfo;
	};

	struct LinPhysicalValue
	{
		uint32_t _maxValue = 0;
		uint32_t _minValue = 0;
		double _offset = 0.0;
		double _scale = 0.0;
		std::string _textInfo;
	};

	using LinValue = std::variant<LinAsciiValue, LinBcdValue, LinLogicalValue, LinPhysicalValue>;

	struct LinSignalEncodingType
	{
		std::string _name;
		std::vector<LinValue> _values;
	};

}
