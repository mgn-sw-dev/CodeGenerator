
#include <OptiScan/Parser/Dbc/DbcDatabase.h>

namespace OptiScan::Parser::Dbc
{
	bool DbcBitTiming::hasNonZero() const
	{
		return this->_baudrate != 0
			|| this->_btr1 != 0
			|| this->_btr2 != 0
		;
	}

	DbcMessage::DbcMessage()
	: _id(0)
	, _name()
	, _size_byte(0)
	{
	}

	uint32_t DbcMessage::extendedCanId() const
	{
		return this->_id & DbcMessage::ExtendedCanIdMask;
	}

	bool DbcMessage::isExtendedCanId() const
	{
		return 0 != (this->_id & ~DbcMessage::ExtendedCanIdMask);
	}

	void DbcMessage::setExtendedCanId(uint32_t id)
	{
		this->_id = id | ~DbcMessage::ExtendedCanIdMask;
	}
}
