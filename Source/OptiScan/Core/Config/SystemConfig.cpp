
#include <OptiScan/Core/Config/SystemConfig.h>
#include <OptiScan/Core/Json/JsonSchemaValidator.h>
#include <fstream>

using namespace nlohmann;
using namespace std;

namespace OptiScan::Core::Config
{
	SystemConfig::SystemConfig()
		: _logHandler(nullptr)
	{
	}

	void SystemConfig::loadConfigFile(const string & configPath, json & jsonRootObject) const
	{
		ifstream file(configPath);
		if (!file.is_open())
		{
			this->logError("Failed to open file: " + configPath);
			throw runtime_error("Failed to open file: " + configPath);
		}
		try
		{
			file >> jsonRootObject;
		}
		catch (const json::parse_error & error)
		{
			this->logError("JSON Parse Error: " + string(error.what()));
			throw runtime_error("JSON Parse Error: " + string(error.what()));
		}
	}

	bool SystemConfig::loadFromFile(const string & configPath)
	{
		bool result = true;
		this->resetAllFields();
		this->logInfo("Loading configuration file: " + configPath);
		json jsonRootObject;

		this->loadConfigFile(configPath, jsonRootObject);
		this->logInfo("Validate config file against schema");
		try
		{
			this->validateAgainstSchema(jsonRootObject);
		}
		catch (const exception & error)
		{
			this->logError(error.what());
			throw;
		}
		this->logInfo("Parse config file");

		return result;
	}

	void SystemConfig::logError(const string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->logError(message);
		}
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

	void SystemConfig::validateAgainstSchema(const json & jsonRootObject) const
	{
		Json::JsonSchemaValidator validator(jsonRootObject);
		validator.loadSchemaFromJson();
		validator.validate();
	}
}
