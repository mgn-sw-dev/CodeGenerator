#pragma once

namespace OptiScan::Parser::Ldf
{
	struct LdfModes
	{
		bool _allowSignalEncodingType32 = false;
		bool _allowSignalWithoutSubscriber = false;
		/** */
		LdfModes() = default;
	};
}
