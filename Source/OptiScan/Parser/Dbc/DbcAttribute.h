#pragma once

#include <string>
#include <variant>
#include <vector>

namespace OptiScan::Parser::Dbc
{
	using DbcAttributeValue = std::variant<std::monostate, std::string, uint32_t, int32_t, double>;

	enum class DbcAttributeObjectType
	{
		Unknown,
		EnvVar,
		Network,
		Node,
		Message,
		Signal,
	};

	enum class DbcAttributeValueType
	{
		Unknown,
		Enum,
		Float,
		Hex,
		Int,
		String,
	};

	struct DbcAttribute
	{
		std::string _name;
		DbcAttributeValue _value;
	};

	struct DbcAttributeDefEnum
	{
		std::vector<std::string> _values;
	};

	struct DbcAttributeDefFloat
	{
		double _maximum = 0.0;
		double _minimum = 0.0;
	};

	struct DbcAttributeDefHex
	{
		uint32_t _maximum = 0;
		uint32_t _minimum = 0;
	};

	struct DbcAttributeDefInt
	{
		int32_t _maximum = 0;
		int32_t _minimum = 0;
	};

	struct DbcAttributeDefString
	{
	};

	using DbcAttributeDefVariant = std::variant<
		DbcAttributeDefEnum,
		DbcAttributeDefFloat,
		DbcAttributeDefHex,
		DbcAttributeDefInt,
		DbcAttributeDefString
	>;

	struct DbcAttributeDef
	{
		std::string _name;
		DbcAttributeObjectType _objectType = DbcAttributeObjectType::Unknown;
		DbcAttributeDefVariant _def;
	};

	struct DbcAttributeEnvVar
	{
		std::string _envVarName;
		std::string _name;
		DbcAttributeValue _value;
	};

	struct DbcAttributeMessage
	{
		uint32_t _messageId = 0;
		std::string _name;
		DbcAttributeValue _value;
	};

	struct DbcAttributeNode
	{
		std::string _name;
		std::string _nodeName;
		DbcAttributeValue _value;
	};

	struct DbcAttributeSignal
	{
		uint32_t _messageId = 0;
		std::string _name;
		std::string _signalName;
		DbcAttributeValue _value;
	};

}
