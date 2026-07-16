
#include <OptiScan/Core/Config/ConfigDatabaseCan.h>

using namespace OptiScan::Core::Common;
using namespace std;

namespace OptiScan::Core::Config
{
	CanBusObject::CanBusObject()
		: BusObject()
		, _a2lName()
		, _baudRate(0)
		, _dataBaudRate(0)
		, _dbcNames()
		, _handledMessages()
		, _type(CanBusType::Unknown)
		, _termination()
		, _transmitting()
		, _transportLayerInstance()
		, _voiceToCanVersion()
	{
	}

	void CanBusObject::clear()
	{
		this->BusObject::clear();
		this->_a2lName.clear();
		this->_baudRate = 0;
		this->_dataBaudRate = 0;
		this->_dbcNames.clear();
		this->_handledMessages.reset();
		this->_type = CanBusType::Unknown;
		this->_termination .reset();
		this->_transmitting.reset();
		this->_transportLayerInstance.clear();
		this->_voiceToCanVersion = Version();
	}

	bool CanBusObject::isValid() const
	{
		bool result = false;
		result = this->_baudRate > 0 && this->_hardwareId > 0 && this->_name.size() > 0;
		if (result)
		{
			switch (this->_type)
			{
			case CanBusType::Standard:
				result = this->_dbcNames.size() > 0;
				break;
			case CanBusType::Fd:
				result = this->_dataBaudRate > 0 &&
						this->_dbcNames.size() > 0;
				break;
			case CanBusType::VoiceToCan:
				result = this->_voiceToCanVersion != Version() &&
						this->_termination.has_value() &&
						this->_transmitting.has_value();
				break;
			case CanBusType::Xcp:
				result = this->_a2lName.size() > 0 &&
						this->_transmitting.has_value();
				break;
			case CanBusType::XcpPlus:
				result = this->_a2lName.size() > 0 &&
						this->_transportLayerInstance.size() > 0 &&
						this->_transmitting.has_value();
				break;
			case CanBusType::Unknown:
			default:
				break;
			}
		}
		return result;
	}

	void CanBusObject::setCanType(const CanBusType & type)
	{
		this->_type = type;
	}

	void CanBusObject::setCanType(const string & typeString)
	{
		CanBusType type;
		if (typeString == "Standard")
		{
			type = CanBusType::Standard;
		}
		else if (typeString == "Fd")
		{
			type = CanBusType::Fd;
		}
		else if (typeString == "Xcp")
		{
			type = CanBusType::Xcp;
		}
		else if (typeString == "XcpPlus")
		{
			type = CanBusType::XcpPlus;
		}
		else if (typeString == "VoiceToCan")
		{
			type = CanBusType::VoiceToCan;
		}
		else
		{
			type = CanBusType::Unknown;
		}
		this->setCanType(type);
	}

}
