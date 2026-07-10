#pragma once

#include <OptiScan/Core/Common/Version.h>
#include <OptiScan/Core/Config/ConfigDatabaseTrigger.h>
#include <string>
#include <vector>

namespace OptiScan::Core::Config
{
	class StandardTrace
	{
	public:
		std::vector<std::string> _busses;
		double _frequency_Hz = 0.0;
		uint8_t _prefetchTime_s = 0;
		uint8_t _recordTime_s = 0;
		std::vector<Trigger> _triggers;
		Common::Version _version;

		StandardTrace() = default;
	};
}
