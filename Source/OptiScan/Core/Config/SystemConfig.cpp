
#include <OptiScan/Core/Config/SystemConfig.h>
#include <OptiScan/Core/Config/SystemConfigConstants.h>
#include <OptiScan/Core/Json/JsonSchemaValidator.h>
#include <fstream>


using namespace OptiScan::Core::Common;
using namespace nlohmann;
using namespace std;

namespace OptiScan::Core::Config
{
	SystemConfig::SystemConfig()
		: _jsonRootObject()
		, _logHandler(nullptr)
	{
	}

	void SystemConfig::loadConfigFile(const string & configPath, json & jsonRootObject) const
	{
		ifstream file(configPath);
		if (!file.is_open())
		{
			throw runtime_error("Failed to open file: " + configPath);
		}
		try
		{
			file >> jsonRootObject;
		}
		catch (const json::parse_error & error)
		{
			throw runtime_error("JSON Parse Error: " + string(error.what()));
		}
	}

	void SystemConfig::loadFromFile(const string & configPath)
	{
		this->resetAllFields();
		this->logInfo("Loading configuration file: " + configPath);
		try
		{
			this->loadConfigFile(configPath, this->_jsonRootObject);
		}
		catch (const exception & error)
		{
			this->logError(error.what());
			throw;
		}
		this->logInfo("Validate config file against schema");
		try
		{
			this->validateAgainstSchema();
		}
		catch (const exception & error)
		{
			this->logError(error.what());
			throw runtime_error(error.what());
		}
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

	void SystemConfig::parse(ConfigDatabase & configDatabase)
	{
		if (this->_jsonRootObject.is_null())
		{
			throw runtime_error("Config file is not loaded. Call loadFromFile() first.");
		}
		this->logInfo("Parse config file");

		// Through schema required parameters
		this->parseProjectInfoObject(configDatabase);
		this->logInfo("Project infos:"
			  "\n\t- ProjectName: " + configDatabase._projectInfos._projectName
			+ "\n\t- SystemName: " + configDatabase._projectInfos._systemName
			+ "\n\t- SystemVersion: " + configDatabase._projectInfos._systemVersion.toString()
			+ "\n\t- Generation: " + to_string(configDatabase._projectInfos._generation));
		this->parseDebugObject(configDatabase);
		this->logInfo("Debug object:"
			  "\n\t- Frequency: " + LogHandler::doubleToString(configDatabase._debugObject._frequency_Hz, 2) + " Hz"
			+ "\n\t- Include VIN: " + (configDatabase._debugObject._vinIncluded ? "true" : "false"));
		this->parseGpsObject(configDatabase);
		this->logInfo("GPS object: "
			  "\n\t- Frequency: " + LogHandler::doubleToString(configDatabase._gpsObject._frequency_Hz, 2) + " Hz");

	}

	void SystemConfig::parseDebugObject(ConfigDatabase & configDatabase)
	{
		const json & debugObject = this->_jsonRootObject[SystemConfigConstants::Debug];
		configDatabase._debugObject._frequency_Hz = debugObject.at(SystemConfigConstants::Frequency_Hz);
		configDatabase._debugObject._vinIncluded = debugObject.at(SystemConfigConstants::IncludeVin);
	}

	void SystemConfig::parseGpsObject(ConfigDatabase & configDatabase)
	{
		const json & gpsObject = this->_jsonRootObject[SystemConfigConstants::Gps];
		configDatabase._gpsObject._frequency_Hz = gpsObject.at(SystemConfigConstants::Frequency_Hz);
	}

	void SystemConfig::parseProjectInfoObject(ConfigDatabase & configDatabase)
	{
		const json & projectInfoObject = this->_jsonRootObject[SystemConfigConstants::ProjectInfos];
		configDatabase._projectInfos._customer = projectInfoObject.at(SystemConfigConstants::Customer);
		configDatabase._projectInfos._firmwareVersion = Version::fromString(projectInfoObject.at(SystemConfigConstants::FirmwareVersion));
		configDatabase._projectInfos._fleetManagementRelease = projectInfoObject.at(SystemConfigConstants::FleetManagementRelease);
		configDatabase._projectInfos._generation = projectInfoObject.at(SystemConfigConstants::Generation);
		configDatabase._projectInfos._projectName = projectInfoObject.at(SystemConfigConstants::ProjectName);
		configDatabase._projectInfos._systemName = projectInfoObject.at(SystemConfigConstants::SystemName);
		configDatabase._projectInfos._systemVersion = Version::fromString(projectInfoObject.at(SystemConfigConstants::SystemVersion));
	}

	void SystemConfig::resetAllFields()
	{
		this->_jsonRootObject = json();
	}

	void SystemConfig::setLogHandler(LogHandler * logHandler)
	{
		this->_logHandler = logHandler;
	}

	void SystemConfig::validateAgainstSchema() const
	{
		if (!this->_jsonRootObject.is_null())
		{
			Json::JsonSchemaValidator validator(this->_jsonRootObject);
			validator.loadSchemaFromJson();
			validator.validate();
		}
	}
}
