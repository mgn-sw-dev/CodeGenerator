
#include <OptiScan/Core/Database/CanDatabase.h>
#include <fstream>

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
		// ToDo: parse dbc file to _dbcDatabase
	}

}
