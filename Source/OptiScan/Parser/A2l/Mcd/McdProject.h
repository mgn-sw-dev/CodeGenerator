#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdHeader.h>
#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <OptiScan/Parser/A2l/Mcd/McdModule.h>
#include <optional>
#include <string>
#include <vector>

namespace OptiScan::Parser::A2l::Mcd
{
	/** Move only, because McdModule is move only. */
	struct McdProject
	{
		std::optional<McdHeader> _header;
		std::string _longIdentifier;
		std::vector<McdModule> _modules;
		McdIdent _name;
	};

}
