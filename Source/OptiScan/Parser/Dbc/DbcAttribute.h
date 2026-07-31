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

	class DbcAttributeDef
	{
	public:
		std::string _name;
		DbcAttributeObjectType _objectType = DbcAttributeObjectType::Unknown;
		// ToDo: Use variant
		/** */
		virtual ~DbcAttributeDef() = default;
		/** */
		virtual DbcAttributeValueType valueType() const = 0;
	};

	class DbcAttributeDefEnum : public DbcAttributeDef
	{
	public:
		std::vector<std::string> _values;

		virtual DbcAttributeValueType valueType() const override;
	};

	class DbcAttributeDefFloat : public DbcAttributeDef
	{
	public:
		double _maximum;
		double _minimum;
		/** */
		virtual DbcAttributeValueType valueType() const override;
	};

	class DbcAttributeDefHex : public DbcAttributeDef
	{
	public:
		uint32_t _maximum;
		uint32_t _minimum;
		/** */
		virtual DbcAttributeValueType valueType() const override;
	};

	class DbcAttributeDefInt : public DbcAttributeDef
	{
	public:
		int32_t _maximum;
		int32_t _minimum;
		/** */
		virtual DbcAttributeValueType valueType() const override;
	};

	class DbcAttributeDefString : public DbcAttributeDef
	{
	public:
		/** */
		virtual DbcAttributeValueType valueType() const override;
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
