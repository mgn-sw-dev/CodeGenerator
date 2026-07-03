#include <OptiScan/Core/Config/SystemConfig.h>

using namespace std;

namespace OptiScan::Core::Config
{
	SystemConfig::SystemConfig()
		: _logHandler(nullptr)
	{
	}

	bool SystemConfig::loadFromFile(const string & configPath)
	{
		this->logInfo("Loading configuration file: " + configPath);
		return true;
	}

	void SystemConfig::logInfo(const string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->logInfo(message);
		}
	}

	void SystemConfig::resetAllFields()
	{
	}

	void SystemConfig::setLogHandler(LogHandler * logHandler)
	{
		this->_logHandler = logHandler;
	}
}
