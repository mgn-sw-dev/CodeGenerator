
#include <OptiScan/Parser/FileException.h>

namespace OptiScan::Parser
{
	FormatException::FormatException(const std::string & message)
		: std::runtime_error(message)
	{
	}

	InvalidOperationException::InvalidOperationException(const std::string & message)
		: std::logic_error(message)
	{
	}
}
