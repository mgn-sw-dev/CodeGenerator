
#include <OptiScan/Parser/Dbc/DbcException.h>

namespace OptiScan::Parser::Dbc
{
	DbcFormatException::DbcFormatException(const std::string & message)
		: std::runtime_error(message)
	{
	}

	DbcInvalidOperationException::DbcInvalidOperationException(const std::string & message)
		: std::logic_error(message)
	{
	}
}
