
#include <OptiScan/Parser/Dbc/DbcFormatException.h>

namespace OptiScan::Parser::Dbc
{
	DbcFormatException::DbcFormatException(const std::string & message)
		: std::runtime_error(message)
	{
	}
}
