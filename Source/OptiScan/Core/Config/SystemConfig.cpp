#include <OptiScan/Core/Config/SystemConfig.h>

using namespace std;

namespace OptiScan::Core::Config
{
	SystemConfig::SystemConfig()
	{
	}

	bool SystemConfig::loadFromFile(const string & configPath)
	{
		this->log("Loading configuration file: " + configPath);
		return true;
	}

	void SystemConfig::log(const string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler(message);
		}
	}

	void SystemConfig::resetAllFields()
	{
	}

	void SystemConfig::setLogHandler(LogHandler logHandler)
	{
		this->_logHandler = std::move(logHandler);
	}
}
