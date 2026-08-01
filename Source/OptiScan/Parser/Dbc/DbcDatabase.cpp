
#include <OptiScan/Parser/Dbc/DbcDatabase.h>

namespace OptiScan::Parser::Dbc
{

	DbcAttributeValueType DbcAttributeDefEnum::valueType() const
	{
		return DbcAttributeValueType::Enum;
	}

	DbcAttributeValueType DbcAttributeDefFloat::valueType() const
	{
		return DbcAttributeValueType::Float;
	}

	DbcAttributeValueType DbcAttributeDefHex::valueType() const
	{
		return DbcAttributeValueType::Hex;
	}

	DbcAttributeValueType DbcAttributeDefInt::valueType() const
	{
		return DbcAttributeValueType::Int;
	}

	DbcAttributeValueType DbcAttributeDefString::valueType() const
	{
		return DbcAttributeValueType::String;
	}

}
