#pragma once

#include <OptiScan/Parser/A2l/A2lScanner.h>

namespace OptiScan::Parser::A2l
{
	/** Settings applied to the parser before @ref A2lParser::parse is called. */
	struct A2lOptions
	{
		A2lScannerCompatStyles _compatStyles;
		/** */
		A2lOptions() = default;
	};

}
