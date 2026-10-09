
#include <OptiScan/Parser/A2l/Mcd/McdCompuMethod.h>

namespace OptiScan::Parser::A2l::Mcd
{
	McdCoeffs::McdCoeffs()
		: _a(0.0)
		, _b(0.0)
		, _c(0.0)
		, _d(0.0)
		, _e(0.0)
		, _f(0.0)
	{
	}

	McdCoeffsLinear::McdCoeffsLinear()
		: _a(0.0)
		, _b(0.0)
	{
	}

	McdCompuMethod::McdCompuMethod()
		: _coeffs()
		, _coeffsLinear()
		, _compuTabRef()
		, _conversionType(McdConversionType::Identical)
		, _format()
		, _formula()
		, _longIdentifier()
		, _name()
		, _physicalUnit()
		, _refUnit()
		, _statusStringRef()
	{
	}

	McdCompuTab::McdCompuTab()
		: _conversionType(McdConversionType::Identical)
		, _defaultValue()
		, _longIdentifier()
		, _name()
		, _valuePairs()
	{
	}

	McdCompuVTab::McdCompuVTab()
		: _conversionType(McdConversionType::Identical)
		, _defaultValue()
		, _longIdentifier()
		, _name()
		, _valuePairs()
	{
	}

}
