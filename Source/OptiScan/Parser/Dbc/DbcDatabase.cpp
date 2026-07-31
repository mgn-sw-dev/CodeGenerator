
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
}
