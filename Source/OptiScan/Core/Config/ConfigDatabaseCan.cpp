
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
		, _termination(false)
		, _transmitting(false)
	{
	}

	void CanBusObject::clear()
	{
		this->BusObject::clear();
		this->_baudRate = 0;
		this->_termination = false;
		this->_transmitting = false;
	}

	CanStandardBusObject::CanStandardBusObject(const CanBusType & type)
		: CanBusObject(type)
		, _dbcNames()
		, _handledMessages()
	{
	}

	void CanStandardBusObject::clear()
	{
		this->CanBusObject::clear();
		this->_dbcNames.clear();
		this->_handledMessages.reset();
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

	CanVoiceToCanBusObject::CanVoiceToCanBusObject(const CanBusType & type)
		: CanBusObject(type)
		, _version()
	{
	}

	void CanVoiceToCanBusObject::clear()
	{
		this->CanBusObject::clear();
		this->_version = Common::Version();
	}

	CanXcpBusObject::CanXcpBusObject(const CanBusType & type)
		: CanBusObject(type)
		, _a2lName()
	{
	}

	void CanXcpBusObject::clear()
	{
		this->CanBusObject::clear();
		this->_a2lName.clear();
	}

	CanXcpPlusBusObject::CanXcpPlusBusObject(const CanBusType & type)
		: CanXcpBusObject(type)
		, _transportLayerInstance()
	{
	}

	void CanXcpPlusBusObject::clear()
	{
		this->CanXcpBusObject::clear();
		this->_transportLayerInstance.clear();
	}

}
