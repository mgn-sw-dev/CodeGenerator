#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdProject.h>
#include <OptiScan/Parser/A2l/Mcd/McdVersion.h>
#include <optional>

namespace OptiScan::Parser::A2l
{
	/** Move only, because Mcd::McdProject is move only. */
	class A2lDatabase
	{
	public:
		std::optional<Mcd::McdVersion> _a2mlVersion;
		Mcd::McdVersion _asap2Version;
		Mcd::McdProject _project;
	};

}
