#pragma once

#include <stdexcept>
#include <string>

namespace OptiScan::Parser
{

	class FormatException : public std::runtime_error
	{
	public:
		explicit  FormatException(const std::string & message);
	};

	class InvalidOperationException : public std::logic_error
	{
	public:
		explicit  InvalidOperationException(const std::string & message);
	};

}
