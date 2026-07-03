#pragma once

#include <string>

namespace OptiScan::Core
{
	class LogHandler
	{
	public:
		virtual ~LogHandler() = default;

		virtual void logInfo(const std::string& message) = 0;
		virtual void logWarning(const std::string& message) = 0;
		virtual void logError(const std::string& message) = 0;

		static std::string doubleToString(double value, int precision = 2);
	};
}