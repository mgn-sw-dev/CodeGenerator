#pragma once

#include <cstdint>

namespace OptiScan::Parser::A2l::Mcd
{
	struct McdVersion
	{
		uint16_t _upgradeNo = 0;
		uint16_t _versionNo = 0;
	};

}
