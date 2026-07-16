
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

	void SystemConfig::loadConfigFile(const string & configPath, json & jsonRootObject)
	{
		ifstream file(configPath);
		if (!file.is_open())
		{
			throw runtime_error("SystemConfig::loadConfigFile:Failed to open file: " + configPath);
		}
		try
		{
			file >> jsonRootObject;
		}
		catch (const json::parse_error & error)
		{
			throw runtime_error("SystemConfig::loadConfigFile: JSON Parse Error: " + string(error.what()));
		}
	}

	void SystemConfig::loadFromFile(const string & configPath)
	{
		this->reset();
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
			this->logError("SystemConfig::loadFromFile: " + string(error.what()));
			throw runtime_error(error.what());
		}
	}

	void SystemConfig::log(const string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->log(message);
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
			this->logError("SystemConfig::parse: Config file is not loaded. Call loadFromFile() first.");
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
			this->logInfo("Can object:");
			this->parseCanObject(configDatabase);
		}
		if (this->_jsonRootObject.contains(SystemConfigConstants::Lin))
		{
			this->logInfo("Lin object:");
			this->parseLinObject(configDatabase);
		}
	}

	uint32_t SystemConfig::parseHexStringAsUInt32(const std::string & hexString)
	{
		uint32_t result = 0;
		try
		{
			size_t parsedChars = 0;
			const unsigned long value = stoul(hexString, &parsedChars, 16);
			if (parsedChars != hexString.size() || value > numeric_limits<uint32_t>::max())
			{
				throw invalid_argument("Invalid uint32 hex string: " + hexString);
			}
			result = static_cast<uint32_t>(value);
		}
		catch (const exception & e)
		{
			throw runtime_error("Failed to parse hex string: " + hexString + ". Error: " + e.what());
		}
		return result;
	}

	void SystemConfig::parseCanBusHandledMessagesObject(const json & jsonObject, CanHandledMessagesObject & canHandledMessages)
	{
		if (jsonObject.contains(SystemConfigConstants::Gps))
		{
			canHandledMessages._gps.emplace();
			const json & gpsObject = jsonObject.at(SystemConfigConstants::Gps);
			if (gpsObject.contains(SystemConfigConstants::Altitude))
			{
				const json & altitudeObject = gpsObject.at(SystemConfigConstants::Altitude);
				SystemConfig::parseArrayWith(altitudeObject, canHandledMessages._gps->_altitude, &SystemConfig::parseCanMessageSignalMap);
			}
			if (gpsObject.contains(SystemConfigConstants::Latitude))
			{
				const json & latitudeObject = gpsObject.at(SystemConfigConstants::Latitude);
				SystemConfig::parseArrayWith(latitudeObject, canHandledMessages._gps->_latitude, &SystemConfig::parseCanMessageSignalMap);
			}
			if (gpsObject.contains(SystemConfigConstants::Longitude))
			{
				const json & longitudeObject = gpsObject.at(SystemConfigConstants::Longitude);
				SystemConfig::parseArrayWith(longitudeObject, canHandledMessages._gps->_longitude, &SystemConfig::parseCanMessageSignalMap);
			}
		}
		if (jsonObject.contains(SystemConfigConstants::Vin))
		{
			canHandledMessages._vin.emplace();
			const json & vinObject = jsonObject.at(SystemConfigConstants::Vin);
			canHandledMessages._vin->_messageName = vinObject.at(SystemConfigConstants::MessageName).get<string>();
			canHandledMessages._vin->_signalName = vinObject.at(SystemConfigConstants::SignalName).get<string>();
		}
	}

	void SystemConfig::parseCanBusObject(const json & busElement, CanBusObject & canBusObject)
	{
		canBusObject.clear();
		SystemConfig::parseCommonCanBusFields(busElement, canBusObject);
		canBusObject.setCanType(busElement.at(SystemConfigConstants::Type).get<string>());
		switch (canBusObject._type)
		{
		case CanBusType::Fd:
			canBusObject._dataBaudRate = busElement.at(SystemConfigConstants::DataBaudRate).get<uint32_t>();
			SystemConfig::parseStandardCanBusFields(busElement, canBusObject);
			break;
		case CanBusType::Standard:
			SystemConfig::parseStandardCanBusFields(busElement, canBusObject);
			break;
		case CanBusType::VoiceToCan:
			canBusObject._voiceToCanVersion = Version::fromString(busElement.at(SystemConfigConstants::Version).get<string>());
			break;
		case CanBusType::Xcp:
			canBusObject._a2lName = busElement.at(SystemConfigConstants::A2lName).get<string>();
			break;
		case CanBusType::XcpPlus:
			canBusObject._a2lName = busElement.at(SystemConfigConstants::A2lName).get<string>();
			canBusObject._transportLayerInstance = busElement.at(SystemConfigConstants::TransportLayerInstance).get<string>();
			break;
		case CanBusType::Unknown:
		default:
			break;
		}
		if (!canBusObject.isValid())
		{
			throw runtime_error("SystemConfig::parseCanBusObject: Invalid can bus object.");
		}
	}

	void SystemConfig::parseCanMessageSignalMap(const json & signalElement, CanMessageSignalMap & canMessageSignalMap)
	{
		canMessageSignalMap._messageName = signalElement.at(SystemConfigConstants::MessageName).get<string>();
		canMessageSignalMap._signalName = signalElement.at(SystemConfigConstants::SignalName).get<string>();
	}

	void SystemConfig::parseCanObject(ConfigDatabase & configDatabase)
	{
		const json & canObject = this->_jsonRootObject[SystemConfigConstants::Can];
		configDatabase._canObject.emplace();
		SystemConfig::parseArrayWith(canObject.at(SystemConfigConstants::Busses), configDatabase._canObject->_busses, &SystemConfig::parseCanBusObject);
		this->log("\t- Busses: " + to_string(configDatabase._canObject->_busses.size()));
		if (canObject.contains(SystemConfigConstants::StandardTrace))
		{
			configDatabase._canObject->_standardTrace.emplace();
			this->parseStandardTrace(canObject.at(SystemConfigConstants::StandardTrace), *configDatabase._canObject->_standardTrace);
			this->log("\t- Standard Trace:"
				"\n\t\t- Busses: " + to_string(configDatabase._canObject->_standardTrace->_busses.size())
				+"\n\t\t- PrefetchTime: " + to_string(configDatabase._canObject->_standardTrace->_prefetchTime_s)
				+"\n\t\t- RecordTime: " + to_string(configDatabase._canObject->_standardTrace->_recordTime_s)
				+"\n\t\t- Trigger: " + to_string(configDatabase._canObject->_standardTrace->_triggers.size()));
		}
		SystemConfig::parseOptionalValue<double>(canObject, SystemConfigConstants::FrequencyMax_Hz, configDatabase._canObject->_frequencyMax_Hz);
		SystemConfig::parseOptionalValue<string>(canObject, SystemConfigConstants::SelectionTable, configDatabase._canObject->_selectionTable);
		SystemConfig::parseOptionalValue<double>(canObject, SystemConfigConstants::XcpFrequencyMax_Hz, configDatabase._canObject->_xcpFrequencyMax_Hz);
		SystemConfig::parseOptionalValue<string>(canObject, SystemConfigConstants::XcpSelectionTable, configDatabase._canObject->_xcpSelectionTable);

		if (configDatabase._canObject->_frequencyMax_Hz.has_value())
		{
			this->log("\t- Frequency Max Hz: " + LogHandler::doubleToString(configDatabase._canObject->_frequencyMax_Hz.value()));
		}
		if (configDatabase._canObject->_selectionTable.has_value())
		{
			this->log("\t- Selection Table: " + configDatabase._canObject->_selectionTable.value());
		}
		if (configDatabase._canObject->_xcpFrequencyMax_Hz.has_value())
		{
			this->log("\t- XCP Frequency Max Hz: " + LogHandler::doubleToString(configDatabase._canObject->_xcpFrequencyMax_Hz.value()));
		}
		if (configDatabase._canObject->_xcpSelectionTable.has_value())
		{
			this->log("\t- XCP Selection Table: " + configDatabase._canObject->_xcpSelectionTable.value());
		}
	}

	void SystemConfig::parseCanSignals(const json & signalsElement, CanTriggerSignal & canTriggerSignal)
	{
		canTriggerSignal._bus = signalsElement.at(SystemConfigConstants::Bus).get<string>();
		canTriggerSignal._dbcName = signalsElement.at(SystemConfigConstants::DbcName).get<string>();
		canTriggerSignal._messageId = SystemConfig::parseHexStringAsUInt32(signalsElement.at(SystemConfigConstants::MessageId).get<string>());
		canTriggerSignal._signalName = signalsElement.at(SystemConfigConstants::SignalName).get<string>();
	}

	void SystemConfig::parseCommonCanBusFields(const json & busElement, CanBusObject & canBusObject)
	{
		canBusObject._baudRate = busElement.at(SystemConfigConstants::BaudRate).get<uint32_t>();
		canBusObject._hardwareId = busElement.at(SystemConfigConstants::Hardware).get<uint8_t>();
		canBusObject._name = busElement.at(SystemConfigConstants::Id).get<string>();
		canBusObject.setIdSuffix("Can");
		SystemConfig::parseOptionalValue<bool>(busElement, SystemConfigConstants::Termination, canBusObject._termination);
		SystemConfig::parseOptionalValue<bool>(busElement, SystemConfigConstants::Transmitting, canBusObject._transmitting);
	}

	void SystemConfig::parseDebugObject(ConfigDatabase & configDatabase)
	{
		const json & debugObject = this->_jsonRootObject[SystemConfigConstants::Debug];
		configDatabase._debugObject._frequency_Hz = debugObject.at(SystemConfigConstants::Frequency_Hz).get<double>();
		configDatabase._debugObject._vinIncluded = debugObject.at(SystemConfigConstants::IncludeVin).get<bool>();
	}

	void SystemConfig::parseGpsObject(ConfigDatabase & configDatabase)
	{
		const json & gpsObject = this->_jsonRootObject[SystemConfigConstants::Gps];
		configDatabase._gpsObject._frequency_Hz = gpsObject.at(SystemConfigConstants::Frequency_Hz).get<double>();
	}

	void SystemConfig::parseLinObject(ConfigDatabase & configDatabase)
	{
		const json & linObject = this->_jsonRootObject[SystemConfigConstants::Lin];
		configDatabase._linObject.emplace();
		SystemConfig::parseArrayWith(linObject.at(SystemConfigConstants::Busses), configDatabase._linObject->_busses, &SystemConfig::parseLinBusObject);
		this->log("\t- Busses: " + to_string(configDatabase._linObject->_busses.size()));
		configDatabase._linObject->_selectionTable = linObject.at(SystemConfigConstants::SelectionTable).get<string>();
		this->log("\t- Selection Table: " + configDatabase._linObject->_selectionTable);
	}

	void SystemConfig::parseLinBusObject(const json & busElement, LinBusObject & linBusObject)
	{
		linBusObject.clear();
		linBusObject._ldfName = busElement.at(SystemConfigConstants::LdfName).get<string>();
		linBusObject._hardwareId = busElement.at(SystemConfigConstants::Hardware).get<uint8_t>();
		linBusObject._name = busElement.at(SystemConfigConstants::Id).get<string>();
		linBusObject.setIdSuffix("Lin");
	}

	void SystemConfig::parseProjectInfoObject(ConfigDatabase & configDatabase)
	{
		const json & projectInfoObject = this->_jsonRootObject[SystemConfigConstants::ProjectInfos];
		configDatabase._projectInfos._customer = projectInfoObject.at(SystemConfigConstants::Customer).get<string>();
		configDatabase._projectInfos._firmwareVersion = Version::fromString(projectInfoObject.at(SystemConfigConstants::FirmwareVersion).get<string>());
		configDatabase._projectInfos._fleetManagementRelease = projectInfoObject.at(SystemConfigConstants::FleetManagementRelease).get<string>();
		configDatabase._projectInfos._generation = projectInfoObject.at(SystemConfigConstants::Generation).get<uint8_t>();
		configDatabase._projectInfos._projectName = projectInfoObject.at(SystemConfigConstants::ProjectName).get<string>();
		configDatabase._projectInfos._systemName = projectInfoObject.at(SystemConfigConstants::SystemName).get<string>();
		configDatabase._projectInfos._systemVersion = Version::fromString(projectInfoObject.at(SystemConfigConstants::SystemVersion).get<string>());
	}

	void SystemConfig::parseStandardCanBusFields(const json & busElement, CanBusObject & canBusObject)
	{
		SystemConfig::parseArrayAs<string>(busElement.at(SystemConfigConstants::DbcNames), canBusObject._dbcNames);
		if (busElement.contains(SystemConfigConstants::HandledMessages))
		{
			canBusObject._handledMessages.emplace();
			SystemConfig::parseCanBusHandledMessagesObject(busElement.at(SystemConfigConstants::HandledMessages), *canBusObject._handledMessages);
		}
	}

	void SystemConfig::parseStandardTrace(const json & busElement, StandardTrace & standardTrace)
	{
		SystemConfig::parseArrayAs<string>(busElement.at(SystemConfigConstants::Busses), standardTrace._busses);
		standardTrace._frequency_Hz = busElement.at(SystemConfigConstants::Frequency_Hz).get<double>();
		standardTrace._prefetchTime_s = busElement.at(SystemConfigConstants::PrefetchTime_s).get<uint8_t>();
		standardTrace._recordTime_s = busElement.at(SystemConfigConstants::RecordTime_s).get<uint8_t>();
		standardTrace._version = Version::fromString(busElement.at(SystemConfigConstants::Version).get<string>());
		SystemConfig::parseArrayWith(busElement.at(SystemConfigConstants::Trigger), standardTrace._triggers, &SystemConfig::parseTrigger);
	}

	void SystemConfig::parseTrigger(const json & triggerElement, Trigger & trigger)
	{
		trigger.clear();
		trigger.setTriggerType(triggerElement.at(SystemConfigConstants::EventType).get<string>());
		if (triggerElement.contains(SystemConfigConstants::Signals))
		{
			SystemConfig::parseArrayWith(triggerElement.at(SystemConfigConstants::Signals), trigger._signals, &SystemConfig::parseCanSignals);
		}
		switch (trigger._type)
		{
		case TriggerType::MessageIdPrefixPartialMatch:
			{
				trigger._bus = triggerElement.at(SystemConfigConstants::Bus).get<string>();
				trigger._messageId = SystemConfig::parseHexStringAsUInt32(triggerElement.at(SystemConfigConstants::MessageIdMask).get<string>());
			}
			break;
		case TriggerType::MessagePayloadPartialMatch:
		case TriggerType::MessageDtcRotmeldungMatch:
			{
				trigger._bus = triggerElement.at(SystemConfigConstants::Bus).get<string>();
				trigger._messageId = SystemConfig::parseHexStringAsUInt32(triggerElement.at(SystemConfigConstants::MessageIdMask).get<string>());
				trigger._triggerValue = SystemConfig::parseHexStringAsUInt32(triggerElement.at(SystemConfigConstants::TriggerValue).get<string>());
				trigger._triggerValueOffset = triggerElement.at(SystemConfigConstants::TriggerValueOffset).get<uint32_t>();
				trigger._triggerValueSize = triggerElement.at(SystemConfigConstants::TriggerValueSize).get<uint32_t>();
			}
			break;
		case TriggerType::VoiceToCanButton:
		case TriggerType::Unknown:
		default:
			break;
		}
		if (!trigger.isValid())
		{
			throw runtime_error("SystemConfig::parseTrigger: Invalid trigger object.");
		}
	}

	void SystemConfig::reset()
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
