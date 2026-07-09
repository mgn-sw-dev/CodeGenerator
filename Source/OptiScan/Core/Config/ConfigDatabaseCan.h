#pragma once
#include <OptiScan/Core/Config/ConfigDatabaseBus.h>
#include <optional>
#include <stdexcept>
#include <vector>

#include "OptiScan/Core/Common/Version.h"

namespace OptiScan::Core::Config
{
	enum class CanBusType : uint8_t
	{
		Unknown     = 0,
		Standard    = 1,
		Fd          = 2,
		Xcp         = 3,
		XcpPlus     = 4,
		VoiceToCan  = 5,
	};

	inline CanBusType canBusTypeFromString(const std::string & busTypeString)
	{
		CanBusType type;
		if (busTypeString == "Standard")
		{
			type = CanBusType::Standard;
		}
		else if (busTypeString == "Fd")
		{
			type = CanBusType::Fd;
		}
		else if (busTypeString == "Xcp")
		{
			type = CanBusType::Xcp;
		}
		else if (busTypeString == "XcpPlus")
		{
			type = CanBusType::XcpPlus;
		}
		else if (busTypeString == "VoiceToCan")
		{
			type = CanBusType::VoiceToCan;
		}
		else
		{
			throw std::runtime_error("Unknown can bus type: " + busTypeString);
		}
		return type;
	}

	struct CanMessageSignalMap
	{
		std::string _messageName;
		std::string _signalName;
	};

	class CanGpsSignalObject
	{
	public:
		std::vector<CanMessageSignalMap> _altitude;
		std::vector<CanMessageSignalMap> _latitude;
		std::vector<CanMessageSignalMap> _longitude;
	};

	class CanHandledMessagesObject
	{
	public:
		std::optional<CanGpsSignalObject> _gps;
		std::optional<CanMessageSignalMap> _vin;
	};

	class CanBusObject : public BusObject
	{
	public:
		uint32_t _baudRate;
		const CanBusType _type;
		bool _termination;
		bool _transmitting;

		CanBusObject();
		CanBusObject(const CanBusType & type);
		void clear() override;
	};

	class CanStandardBusObject : public CanBusObject
	{
	public:
		std::vector<std::string> _dbcNames;
		std::optional<CanHandledMessagesObject> _handledMessages;

		CanStandardBusObject(const CanBusType & type);
		void clear() override;
	};

	class CanFdBusObject : public CanStandardBusObject
	{
	public:
		uint32_t _dataBaudRate;

		CanFdBusObject(const CanBusType & type);
		void clear() override;
	};

	class CanVoiceToCanBusObject : public CanBusObject
	{
	public:
		Common::Version _version;

		CanVoiceToCanBusObject(const CanBusType & type);
		void clear() override;
	};

	class CanXcpBusObject : public CanBusObject
	{
	public:
		std::string _a2lName;

		CanXcpBusObject(const CanBusType & type);
		void clear() override;
	};

	class CanXcpPlusBusObject : public CanXcpBusObject
	{
	public:
		std::string _transportLayerInstance;

		CanXcpPlusBusObject(const CanBusType & type);
		void clear() override;
	};
}
