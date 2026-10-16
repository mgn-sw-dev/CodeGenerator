
#include <OptiScan/Parser/A2l/Mcd/McdMeasurement.h>

namespace OptiScan::Parser::A2l::Mcd
{
	McdMaxRefresh::McdMaxRefresh()
		: _rate(0)
		, _scalingUnit(0)
	{
	}

	McdSymbolLink::McdSymbolLink()
		: _offset(0)
		, _symbolName()
	{
	}

	McdMeasurement::McdMeasurement()
		: _accuracy(0.0)
		, _addressType(McdAddressType::Direct)
		, _arraySize()
		, _bitMask(UINT64_MAX)
		, _byteOrder(McdByteOrder::None)
		, _conversion()
		, _dataType(McdDataType::None)
		, _displayIdentifier()
		, _ecuAddress()
		, _ecuAddressExtension()
		, _errorMask(0)
		, _format()
		, _ifDatas()
		, _indexMode(McdIndexMode::RowDir)
		, _isDiscrete(false)
		, _isWriteable(false)
		, _longIdentifier()
		, _lowerLimit(0.0)
		, _matrixDim()
		, _maxRefresh()
		, _modelLink()
		, _name()
		, _physicalUnit()
		, _resolution(0)
		, _symbolLink()
		, _upperLimit(0.0)
	{
	}

}
