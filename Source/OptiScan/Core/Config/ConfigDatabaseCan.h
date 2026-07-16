#pragma once

#include <OptiScan/Core/Common/Version.h>
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

	struct CanMessageSignalMap
	{
		std::string _messageName;
		std::string _signalName;
	};

	struct CanGpsSignalObject
	{
		std::vector<CanMessageSignalMap> _altitude;
		std::vector<CanMessageSignalMap> _latitude;
		std::vector<CanMessageSignalMap> _longitude;
	};

	struct CanHandledMessagesObject
	{
		std::optional<CanGpsSignalObject> _gps;
		std::optional<CanMessageSignalMap> _vin;
	};

	class CanBusObject : public BusObject
	{
	public:
		std::string _a2lName;
		uint32_t _baudRate;
		uint32_t _dataBaudRate;
		std::vector<std::string> _dbcNames;
		std::optional<CanHandledMessagesObject> _handledMessages;
		CanBusType _type;
		std::optional<bool> _termination;
		std::optional<bool> _transmitting;
		std::string _transportLayerInstance;
		Common::Version _voiceToCanVersion;

		CanBusObject();
		/** */
		void clear() override;
		/** */
		bool isValid() const;
		/** */
		void setCanType(const CanBusType & type);
		/** if the type string does not match, the type is unknown. */
		void setCanType(const std::string & typeString);
	};

}
