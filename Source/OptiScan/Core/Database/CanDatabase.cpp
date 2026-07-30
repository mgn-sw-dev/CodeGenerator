
#include <OptiScan/Core/Database/CanDatabase.h>
#include <OptiScan/Parser/Dbc/DbcParser.h>
#include <fstream>

using namespace OptiScan::Parser::Dbc;
using namespace std;
using namespace std::filesystem;

namespace OptiScan::Core::Database
{
	CanDatabase::CanDatabase()
		: _dbcDatabase()
		, _name()
	{
	}

	void CanDatabase::clear()
	{
		// this->_dbcDatabase.clear();
		this->_name.clear();
	}

	void CanDatabase::loadDbcDatabase(const path & dbcPath)
	{
		if (!exists(dbcPath))
		{
			throw runtime_error("CanDatabase::loadDbcDatabase: Dbc file '" + dbcPath.string() + "' does not exist.");
		}
		ifstream file(dbcPath, ios::binary);
		if (!file.is_open())
		{
			throw runtime_error("CanDatabase::loadDbcDatabase: Dbc file '" + dbcPath.string() + "' could not be opened.");
		}
		DbcParser parser(&file);
		parser.parse(this->_dbcDatabase);
	}

}
