
#include <OptiScan/Core/Config/ConfigDatabaseCan.h>

namespace OptiScan::Core::Config
{
	CanBusObject::CanBusObject()
		: BusObject()
		, _baudRate()
		, _type(CanBusType::Unknown)
		, _termination()
		, _transmitting()
	{
	}

	CanBusObject::CanBusObject(const CanBusType & type)
		: BusObject()
		, _baudRate()
		, _type(type)
		, _termination()
		, _transmitting()
	{
	}

	void CanBusObject::clear()
	{
		this->BusObject::clear();
		this->_baudRate = 0;
		this->_termination.reset();
		this->_transmitting.reset();
	}

	CanStandardBusObject::CanStandardBusObject(const CanBusType & type)
		: CanBusObject(type)
		, _dbcNames()
		, _gpsMessages()
	{
	}

	void CanStandardBusObject::clear()
	{
		this->CanBusObject::clear();
		this->_dbcNames.clear();
		this->_gpsMessages.reset();
	}

	CanFdBusObject::CanFdBusObject(const CanBusType & type)
		: CanStandardBusObject(type)
		 , _dataBaudRate()
	{
	}

	void CanFdBusObject::clear()
	{
		this->CanStandardBusObject::clear();
		this->_dataBaudRate = 0;
	}

	CanGpsSignalObject::CanGpsSignalObject(const GpsSignalType & gpsSignalType)
	   : _gpsSignalType(gpsSignalType)
	   , _messageAndSignalNames()
	{
	}

	void CanGpsSignalObject::clear()
	{
		this->_messageAndSignalNames.clear();
	}

	CanVoiceToCanBusObject::CanVoiceToCanBusObject(const CanBusType & type)
		: CanBusObject(type)
		, _version()
	{
	}

	void CanVoiceToCanBusObject::clear()
	{
		this->CanBusObject::clear();
		this->_version.clear();
	}

}