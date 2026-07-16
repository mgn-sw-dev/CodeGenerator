
#include <OptiScan/Core/Database/SystemDatabase.h>

using namespace OptiScan::Core::Config;
using namespace std;

namespace OptiScan::Core::Database
{

	SystemDatabase::SystemDatabase()
		: _configDatabase()
		, _logHandler(nullptr)
		, _systemConfig()
	{
	}

	void SystemDatabase::loadFromConfigFile(const string & filePath)
	{
		this->_systemConfig.loadFromFile(filePath);
		if (this->_logHandler != nullptr)
		{
			this->_systemConfig.setLogHandler(this->_logHandler);
		}
		this->_configDatabase.clear();
		this->_systemConfig.parse(this->_configDatabase);
	}

	void SystemDatabase::setLogHandler(LogHandler * logHandler)
	{
		this->_logHandler = logHandler;
	}

}
