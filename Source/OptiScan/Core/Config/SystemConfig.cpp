
#include <OptiScan/Core/Config/SystemConfig.h>
#include <OptiScan/Core/Config/SystemConfigConstants.h>
#include <OptiScan/Core/Json/JsonSchemaValidator.h>
#include <fstream>
#include <memory>


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

		// optional objects
		if (this->_jsonRootObject.contains(SystemConfigConstants::Can))
		{
			this->parseCanObject(configDatabase);
		}

	}

	void SystemConfig::parseCanBusses(ConfigDatabase & configDatabase, const json & canObject)
	{
		const json & canBusses = canObject.at(SystemConfigConstants::Busses);
		// check if canBusses is array is done by schema validation

		configDatabase._canBusObjects.clear();
		configDatabase._canBusObjects.reserve(canBusses.size());
		for (const auto & element : canBusses)
		{
			configDatabase._canBusObjects.push_back(this->parseCanBusObject(element));
		}
	}

	void SystemConfig::parseCanBusDbcArray(const json & dbcArray, vector<string> & dbcNames)
	{
		dbcNames.clear();
		dbcNames.reserve(dbcArray.size());
		for (const auto & element : dbcArray)
		{
			dbcNames.push_back(element);
		}
	}

	void SystemConfig::parseArrayAsMessageSignalMap(const json & array, std::vector<CanMessageSignalMap> & messageSignalMap)
	{
		for (const auto & element : array)
		{
			CanMessageSignalMap messageSignalMapElement;
			messageSignalMapElement._messageName = element.at(SystemConfigConstants::MessageName);
			messageSignalMapElement._signalName = element.at(SystemConfigConstants::SignalName);
			messageSignalMap.push_back(messageSignalMapElement);
		}
	}

	void SystemConfig::parseCanBusHandledMessagesObject(const json & jsonObject, CanHandledMessagesObject & canHandledMessages)
	{
		if (jsonObject.contains(SystemConfigConstants::Gps))
		{
			const json & gpsObject = jsonObject.at(SystemConfigConstants::Gps);
			if (gpsObject.contains(SystemConfigConstants::Altitude))
			{
				canHandledMessages._gps.emplace();
				const json & altitudeObject = gpsObject.at(SystemConfigConstants::Altitude);
				SystemConfig::parseArrayAsMessageSignalMap(altitudeObject, canHandledMessages._gps->_altitude);
			}
			if (gpsObject.contains(SystemConfigConstants::Latitude))
			{
				const json & latitudeObject = gpsObject.at(SystemConfigConstants::Latitude);
				SystemConfig::parseArrayAsMessageSignalMap(latitudeObject, canHandledMessages._gps->_latitude);
			}
			if (gpsObject.contains(SystemConfigConstants::Longitude))
			{
				const json & longitudeObject = gpsObject.at(SystemConfigConstants::Longitude);
				SystemConfig::parseArrayAsMessageSignalMap(longitudeObject, canHandledMessages._gps->_longitude);
			}
		}
		if (jsonObject.contains(SystemConfigConstants::Vin))
		{
			canHandledMessages._vin.emplace();
			const json & vinObject = jsonObject.at(SystemConfigConstants::Vin);
			canHandledMessages._vin->_messageName = vinObject.at(SystemConfigConstants::MessageName);
			canHandledMessages._vin->_signalName = vinObject.at(SystemConfigConstants::SignalName);
		}
	}

	unique_ptr<CanBusObject> SystemConfig::parseCanBusObject(const json & busElement)
	{
		unique_ptr<CanBusObject> canBusObject;
		string idSuffix;
		CanBusType const type = canBusTypeFromString(busElement.at(SystemConfigConstants::Type));
		switch (type)
		{
		case CanBusType::Fd:
			canBusObject = make_unique<CanFdBusObject>(type);
			break;
		case CanBusType::Standard:
			{
				idSuffix = "Can";
				unique_ptr<CanStandardBusObject> canStandardBusObject = make_unique<CanStandardBusObject>(type);
				canStandardBusObject->_baudRate = busElement.at(SystemConfigConstants::BaudRate);
				this->parseCanBusDbcArray(busElement.at(SystemConfigConstants::DbcNames), canStandardBusObject->_dbcNames);
				canStandardBusObject->_hardwareId = busElement.at(SystemConfigConstants::Hardware);
				canStandardBusObject->_name = busElement.at(SystemConfigConstants::Id);
				canStandardBusObject->setIdSuffix(idSuffix);
				// optional parameters
				if (busElement.contains(SystemConfigConstants::Termination))
				{
					canStandardBusObject->_termination = busElement.at(SystemConfigConstants::Termination);
				}
				if (busElement.contains(SystemConfigConstants::Transmitting))
				{
					canStandardBusObject->_transmitting = busElement.at(SystemConfigConstants::Transmitting);
				}
				if (busElement.contains(SystemConfigConstants::HandledMessages))
				{
					canStandardBusObject->_handledMessages.emplace();
					this->parseCanBusHandledMessagesObject(busElement.at(SystemConfigConstants::HandledMessages), *canStandardBusObject->_handledMessages);
				}
				canBusObject = move(canStandardBusObject);
			}
			break;
		case CanBusType::VoiceToCan:
			canBusObject = make_unique<CanVoiceToCanBusObject>(type);
			break;
		case CanBusType::Xcp:
			canBusObject = make_unique<CanBusObject>(type);
			break;
		case CanBusType::XcpPlus:
			canBusObject = make_unique<CanBusObject>(type);
			break;
		default:
			throw runtime_error("Unknown can bus type.");
		}
		return canBusObject;
	}

	void SystemConfig::parseCanObject(ConfigDatabase & configDatabase)
	{
		const json & canObject = this->_jsonRootObject[SystemConfigConstants::Can];
		this->parseCanBusses(configDatabase, canObject);
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
