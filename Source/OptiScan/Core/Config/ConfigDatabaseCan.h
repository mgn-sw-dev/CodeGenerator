#pragma once
#include <OptiScan/Core/Config/ConfigDatabaseBus.h>
#include <optional>
#include <stdexcept>
#include <vector>

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

	enum class GpsSignalType : uint8_t
	{
		Unknown = 0,
		Altitude = 1,
		Latitude = 2,
		Longitude = 3
	};

	class CanGpsSignalObject
	{
	public:
		GpsSignalType _gpsSignalType;
		std::vector<std::pair<std::string, std::string>> _messageAndSignalNames;

		CanGpsSignalObject(const GpsSignalType & gpsSignalType);
		void clear();
	};

	class CanBusObject : public BusObject
	{
	public:
		uint32_t _baudRate;
		const CanBusType _type;
		std::optional<bool> _termination;
		std::optional<bool> _transmitting;

		CanBusObject();
		CanBusObject(const CanBusType & type);
		void clear() override;
	};

	class CanStandardBusObject : public CanBusObject
	{
	public:
		std::vector<std::string> _dbcNames;
		std::optional<std::vector<CanGpsSignalObject>> _gpsMessages;

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
		std::string _version;

		CanVoiceToCanBusObject(const CanBusType & type);
		void clear() override;
	};
}
