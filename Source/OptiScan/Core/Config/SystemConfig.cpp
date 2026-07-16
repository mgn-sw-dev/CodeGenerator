
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
			this->logError("SystemConfig::loadConfigFile: Failed to open file: " + configPath);
			throw runtime_error("Failed to open file: " + configPath);
		}
		try
		{
			file >> jsonRootObject;
		}
		catch (const json::parse_error & error)
		{
			this->logError("SystemConfig::loadConfigFile: JSON Parse Error: " + string(error.what()));
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
			this->logError("SystemConfig::loadFromFile: " + LogHandler::exeptionToString(error));
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

	void SystemConfig::parseCanBusses(const json & canObject, vector<unique_ptr<CanBusObject>> & canBusses)
	{
		const json & busses = canObject.at(SystemConfigConstants::Busses);
		// check if canBusses is array is done by schema validation

		canBusses.clear();
		canBusses.reserve(canBusses.size());
		for (const auto & element : busses)
		{
			canBusses.push_back(this->parseCanBusObject(element));
		}
	}
	
	void SystemConfig::parseArrayAsMessageSignalMap(const json & array, vector<CanMessageSignalMap> & messageSignalMaps)
	{
		messageSignalMaps.clear();
		messageSignalMaps.reserve(array.size());
		for (const auto & element : array)
		{
			CanMessageSignalMap messageSignalMapElement;
			messageSignalMapElement._messageName = element.at(SystemConfigConstants::MessageName);
			messageSignalMapElement._signalName = element.at(SystemConfigConstants::SignalName);
			messageSignalMaps.push_back(messageSignalMapElement);
		}
	}

	void SystemConfig::parseArrayAsString(const json & array, vector<string> & strings)
	{
		strings.clear();
		strings.reserve(array.size());
		for (const auto & element : array)
		{
			strings.push_back(element);
		}
	}

	uint32_t SystemConfig::parseHexStringAsUInt32(const std::string & hexString)
	{
		uint32_t result = 0;
		try
		{
			result = static_cast<uint32_t>(stoul(hexString, nullptr, 16));
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
		CanBusType const type = canBusTypeFromString(busElement.at(SystemConfigConstants::Type));
		switch (type)
		{
		case CanBusType::Fd:
			{
				unique_ptr<CanFdBusObject> canFdBusObject = make_unique<CanFdBusObject>(type);
				canFdBusObject->_dataBaudRate = busElement.at(SystemConfigConstants::DataBaudRate);
				this->parseStandardCanBusFields(busElement, *canFdBusObject);
				canBusObject = move(canFdBusObject);
			}
			break;
		case CanBusType::Standard:
			{
				unique_ptr<CanStandardBusObject> canStandardBusObject = make_unique<CanStandardBusObject>(type);
				this->parseStandardCanBusFields(busElement, *canStandardBusObject);
				canBusObject = move(canStandardBusObject);
			}
			break;
		case CanBusType::VoiceToCan:
			{
				unique_ptr<CanVoiceToCanBusObject> voiceCanObject = make_unique<CanVoiceToCanBusObject>(type);
				voiceCanObject->_version = Version::fromString(busElement.at(SystemConfigConstants::Version));
				canBusObject = move(voiceCanObject);
			}
			break;
		case CanBusType::Xcp:
			{
				unique_ptr<CanXcpBusObject> xcpBusObject = make_unique<CanXcpBusObject>(type);
				xcpBusObject->_a2lName = busElement.at(SystemConfigConstants::A2lName);
				canBusObject = move(xcpBusObject);
			}
			break;
		case CanBusType::XcpPlus:
			{
				unique_ptr<CanXcpPlusBusObject> xcpPlusBusObject = make_unique<CanXcpPlusBusObject>(type);
				xcpPlusBusObject->_a2lName = busElement.at(SystemConfigConstants::A2lName);
				xcpPlusBusObject->_transportLayerInstance = busElement.at(SystemConfigConstants::TransportLayerInstance);
				canBusObject = move(xcpPlusBusObject);
			}
			break;
		default:
			throw runtime_error("Unknown can bus type.");
		}
		this->parseCommonCanBusFields(busElement, *canBusObject);
		return canBusObject;
	}

	void SystemConfig::parseCanObject(ConfigDatabase & configDatabase)
	{
		const json & canObject = this->_jsonRootObject[SystemConfigConstants::Can];
		configDatabase._canObject.emplace();
		this->parseCanBusses(canObject, configDatabase._canObject->_busses);
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
		if (canObject.contains(SystemConfigConstants::FrequencyMax_Hz))
		{
			configDatabase._canObject->_frequencyMax_Hz.emplace();
			configDatabase._canObject->_frequencyMax_Hz = canObject.at(SystemConfigConstants::FrequencyMax_Hz).get<double>();
			this->log("\t- Frequency Max Hz: " + LogHandler::doubleToString(configDatabase._canObject->_frequencyMax_Hz.value()));
		}
		if (canObject.contains(SystemConfigConstants::SelectionTable))
		{
			configDatabase._canObject->_selectionTable.emplace();
			configDatabase._canObject->_selectionTable = canObject.at(SystemConfigConstants::SelectionTable).get<string>();
			this->log("\t- Selection Table: " + configDatabase._canObject->_selectionTable.value());
		}
		if (canObject.contains(SystemConfigConstants::XcpFrequencyMax_Hz))
		{
			configDatabase._canObject->_xcpFrequencyMax_Hz.emplace();
			configDatabase._canObject->_xcpFrequencyMax_Hz = canObject.at(SystemConfigConstants::XcpFrequencyMax_Hz).get<double>();
			this->log("\t- XCP Frequency Max Hz: " + LogHandler::doubleToString(configDatabase._canObject->_xcpFrequencyMax_Hz.value()));
		}
		if (canObject.contains(SystemConfigConstants::XcpSelectionTable))
		{
			configDatabase._canObject->_xcpSelectionTable.emplace();
			configDatabase._canObject->_xcpSelectionTable = canObject.at(SystemConfigConstants::XcpSelectionTable).get<string>();
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
		canBusObject._baudRate = busElement.at(SystemConfigConstants::BaudRate);
		canBusObject._hardwareId = busElement.at(SystemConfigConstants::Hardware);
		canBusObject._name = busElement.at(SystemConfigConstants::Id);
		canBusObject.setIdSuffix("Can");
		if (busElement.contains(SystemConfigConstants::Termination))
		{
			canBusObject._termination = busElement.at(SystemConfigConstants::Termination);
		}
		if (busElement.contains(SystemConfigConstants::Transmitting))
		{
			canBusObject._transmitting = busElement.at(SystemConfigConstants::Transmitting);
		}
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

	void SystemConfig::parseLinObject(ConfigDatabase & configDatabase)
	{
		const json & linObject = this->_jsonRootObject[SystemConfigConstants::Lin];
		configDatabase._linObject.emplace();
		this->parseLinBusses(linObject, configDatabase._linObject->_busses);
		this->log("\t- Busses: " + to_string(configDatabase._linObject->_busses.size()));
		configDatabase._linObject->_selectionTable = linObject.at(SystemConfigConstants::SelectionTable).get<string>();
		this->log("\t- Selection Table: " + configDatabase._linObject->_selectionTable);
	}

	std::unique_ptr<LinBusObject> SystemConfig::parseLinBusObject(const json & busElement)
	{
		LinBusObject linBusObject;
		linBusObject._ldfName = busElement.at(SystemConfigConstants::LdfName).get<string>();
		linBusObject._hardwareId = busElement.at(SystemConfigConstants::Hardware).get<uint8_t>();
		linBusObject._name = busElement.at(SystemConfigConstants::Id).get<string>();
		linBusObject.setIdSuffix("Lin");
		return make_unique<LinBusObject>(linBusObject);
	}

	void SystemConfig::parseLinBusses(const json & linObject, vector<unique_ptr<LinBusObject>> & linBusses)
	{
		const json & busses = linObject.at(SystemConfigConstants::Busses);
		// check if canBusses is array is done by schema validation

		linBusses.clear();
		linBusses.reserve(linBusses.size());
		for (const auto & element : busses)
		{
			linBusses.push_back(this->parseLinBusObject(element));
		}
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

	void SystemConfig::parseStandardCanBusFields(const json & busElement, CanStandardBusObject & canStandardBusObject)
	{
		SystemConfig::parseArrayAsString(busElement.at(SystemConfigConstants::DbcNames), canStandardBusObject._dbcNames);
		if (busElement.contains(SystemConfigConstants::HandledMessages))
		{
			canStandardBusObject._handledMessages.emplace();
			this->parseCanBusHandledMessagesObject(busElement.at(SystemConfigConstants::HandledMessages), *canStandardBusObject._handledMessages);
		}
	}

	void SystemConfig::parseStandardTrace(const json & busElement, StandardTrace & standardTrace)
	{
		SystemConfig::parseArrayAsString(busElement.at(SystemConfigConstants::Busses), standardTrace._busses);
		standardTrace._frequency_Hz = busElement.at(SystemConfigConstants::Frequency_Hz).get<double>();
		standardTrace._prefetchTime_s = busElement.at(SystemConfigConstants::PrefetchTime_s).get<uint8_t>();
		standardTrace._recordTime_s = busElement.at(SystemConfigConstants::RecordTime_s).get<uint8_t>();
		standardTrace._version = Version::fromString(busElement.at(SystemConfigConstants::Version).get<string>());
		const json & triggerObject = busElement.at(SystemConfigConstants::Trigger);
		for (const auto & tiggerElement : triggerObject)
		{
			Trigger trigger;
			this->parseTrigger(tiggerElement, trigger);
			standardTrace._triggers.push_back(trigger);
		}
	}

	void SystemConfig::parseTrigger(const json & triggerElement, Trigger & trigger)
	{
		TriggerType const type = triggerTypeFromString(triggerElement.at(SystemConfigConstants::EventType).get<string>());
		trigger._type = type;
		if (triggerElement.contains(SystemConfigConstants::Signals))
		{
			for (const auto & signalElement : triggerElement.at(SystemConfigConstants::Signals))
			{
				CanTriggerSignal canTriggerSignal;
				this->parseCanSignals(signalElement, canTriggerSignal);
				trigger._signals.push_back(canTriggerSignal);
			}
		}
		switch (type)
		{
		case TriggerType::VoiceToCanButton:
			break;
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
		case TriggerType::Unknown:
		default:
			throw runtime_error("Unknown trigger type.");
		}
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
