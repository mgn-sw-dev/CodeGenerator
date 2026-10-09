#pragma once

#include <OptiScan/Parser/A2l/Mcd/McdIdent.h>
#include <string>

namespace OptiScan::Parser::A2l::Mcd
{
	struct McdHeader
	{
		std::string _comment;
		McdIdent _projectNo;
		std::string _version;
	};

}
