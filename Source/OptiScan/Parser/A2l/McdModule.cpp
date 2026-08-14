

#include <OptiScan/Parser/A2l/McdModule.h>

namespace OptiScan::Parser::A2l
{
	McdModuleCommon::McdModuleCommon()
		: _alignmentByte(1)
		, _alignmentFloat16Ieee(2)
		, _alignmentFloat32Ieee(4)
		, _alignmentFloat64Ieee(8)
		, _alignmentInt64(8)
		, _alignmentLong(4)
		, _alignmentWord(2)
		, _byteOrder(McdByteOrder::MsbLast)
		, _comment()
		, _dataSize(0)
		, _deposit(McdDeposit::Unknown)
	{
	}

}
