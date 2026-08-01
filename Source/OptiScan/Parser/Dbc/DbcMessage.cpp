#include <OptiScan/Parser/Dbc/DbcMessage.h>

namespace OptiScan::Parser::Dbc
{
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
