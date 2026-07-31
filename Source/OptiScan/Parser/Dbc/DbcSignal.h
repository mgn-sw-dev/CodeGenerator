#pragma once

#include <OptiScan/Parser/Flags.h>
#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
	struct DbcExtendedMultiplexing;
	struct DbcMultiplexorValueRange;
	struct DbcSignalEnvVarValueDescriptions;
	struct DbcSignalGroup;
	struct DbcSignalTypeReference;
	struct DbcSignalValueDescriptions;

	enum class DbcMultiplexorIndicator
	{
		None = 0,
		Multiplexor = 1 << 0,
		MultiplexedSignal = 1 << 1,
	};
	using DbcMultiplexorIndicators = Flags<DbcMultiplexorIndicator>;

	struct DbcExtendedMultiplexing
	{
		uint32_t _messageId = 0;
		std::string _multiplexedSignal;
		std::string _multiplexorSwitch;
		std::vector<DbcMultiplexorValueRange> _multiplexorValueRanges;
	};

	struct DbcMultiplexorValueRange
	{
		uint32_t _minimum = 0;
		uint32_t _maximum = 0;
	};

	struct DbcSignalEnvVarValueDescriptions
	{
		std::vector<DbcValueDescription> _descriptions;
		std::string _envVarName;
	};

	struct DbcSignalGroup
	{
		uint32_t _messageId = 0;
		uint32_t _repetitions = 0;
		std::string _name;
		std::vector<std::string> _signalNames;
	};

	struct DbcSignalTypeReference
	{
		uint32_t _messageId = 0;
		std::string _signalName;
		std::string _signalTypeName;
	};

	struct DbcSignalValueDescriptions
	{
		std::vector<DbcValueDescription> _descriptions;
		uint32_t _messageId = 0;
		std::string _signalName;
	};

	class DbcSignalBase
	{
	public:
		double _factor = 0.0;
		bool _isBigEndian = false;
		bool _isSigned = false;
		double _maximum = 0.0;
		double _minimum = 0.0;
		double _offset = 0.0;
		uint32_t _precision = 0;
		uint32_t _size_bit = 0;
		std::string _unit;
	protected:
		/** */
		DbcSignalBase() = default;
	};

	class DbcSignal : public DbcSignalBase
	{
	public:
		DbcMultiplexorIndicators _multiplexorIndicators;
		uint32_t _multiplexorSwitchValue = 0;
		std::string _name;
		std::vector<std::string> _receivers;
		uint32_t _startBit = 0;
		/** */
		DbcSignal() = default;
	};


	class DbcSignalType : public DbcSignalBase
	{
	public:
		double _defaultValue = 0.0;
		std::string _name;
		std::string _valueTable;
		/** */
		DbcSignalType() = default;
	};

}
