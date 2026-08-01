#pragma once

#include <string>
#include <vector>

namespace OptiScan::Parser::Dbc
{
	struct DbcValueDescription
	{
		std::string _description;
		double _value = 0.0;
	};

	struct DbcValueTable
	{
		std::vector<DbcValueDescription> _valueDescriptions;
		std::string _name;
	};
}
