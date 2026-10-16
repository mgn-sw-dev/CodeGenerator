
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpTimeUnit.h>
#include <limits>

using namespace std;

namespace OptiScan::Parser::A2l::IfData::Xcp
{
	XcpTimeUnit const XcpTimeUnit::_1_ns(0);
	XcpTimeUnit const XcpTimeUnit::_10_ns(1);
	XcpTimeUnit const XcpTimeUnit::_100_ns(2);
	XcpTimeUnit const XcpTimeUnit::_1_us(3);
	XcpTimeUnit const XcpTimeUnit::_10_us(4);
	XcpTimeUnit const XcpTimeUnit::_100_us(5);
	XcpTimeUnit const XcpTimeUnit::_1_ms(6);
	XcpTimeUnit const XcpTimeUnit::_10_ms(7);
	XcpTimeUnit const XcpTimeUnit::_100_ms(8);
	XcpTimeUnit const XcpTimeUnit::_1_s(9);

	XcpTimeUnit::XcpTimeUnit(uint8_t id)
		: _id(id)
	{
	}

	uint8_t XcpTimeUnit::id() const
	{
		return this->_id;
	}

	double XcpTimeUnit::to_ms(uint8_t cycle) const
	{
		double result = numeric_limits<double>::quiet_NaN();
		if (this->_id == XcpTimeUnit::_1_ns.id())
		{
			result = cycle * 0.000001;
		}
		else if (this->_id == XcpTimeUnit::_10_ns.id())
		{
			result = cycle * 0.00001;
		}
		else if (this->_id == XcpTimeUnit::_100_ns.id())
		{
			result = cycle * 0.0001;
		}
		else if (this->_id == XcpTimeUnit::_1_us.id())
		{
			result = cycle * 0.001;
		}
		else if (this->_id == XcpTimeUnit::_10_us.id())
		{
			result = cycle * 0.01;
		}
		else if (this->_id == XcpTimeUnit::_100_us.id())
		{
			result = cycle * 0.1;
		}
		else if (this->_id == XcpTimeUnit::_1_ms.id())
		{
			result = cycle * 1.0;
		}
		else if (this->_id == XcpTimeUnit::_10_ms.id())
		{
			result = cycle * 10.0;
		}
		else if (this->_id == XcpTimeUnit::_100_ms.id())
		{
			result = cycle * 100.0;
		}
		else if (this->_id == XcpTimeUnit::_1_s.id())
		{
			result = cycle * 1000.0;
		}
		return result;
	}

}
