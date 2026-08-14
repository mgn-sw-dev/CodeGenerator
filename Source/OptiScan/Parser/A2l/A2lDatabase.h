#pragma once

#include <OptiScan/Parser/A2l/McdIdent.h>
#include <OptiScan/Parser/A2l/McdModule.h>
#include <string>
#include <variant>
#include <vector>
#include <optional>

namespace OptiScan::Parser::A2l
{
	struct McdHeader
	{
		std::string _comment;
		McdIdent _projectNo;
		std::string _version;
	};

	struct McdProject
	{
		std::optional<McdHeader> _header;
		std::string _longIdentifier;
		std::vector<McdModule> _modules;
		McdIdent _name;
	};

	struct McdVersion
	{
		uint16_t _upgradeNo = 0;
		uint16_t _versionNo = 0;
	};


	class A2lDatabase
	{
	public:
		std::optional<McdVersion> _a2mlVersion;
		McdVersion _mcdVersion;
		McdProject _project;
	};
}
