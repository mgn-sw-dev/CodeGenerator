
#include <OptiScan/Parser/Ldf/LdfParser.h>
#include <OptiScan/Core/Database/LinDatabase.h>
#include <fstream>

using namespace OptiScan::Parser::Ldf;
using namespace std;
using namespace std::filesystem;

namespace OptiScan::Core::Database
{
	LinDatabase::LinDatabase()
		: _ldfDatabase()
		, _name()
	{
	}

	void LinDatabase::clear()
	{
		this->_ldfDatabase = LdfDatabase();
		this->_name.clear();
	}

	void LinDatabase::loadLdfDatabase(const path & ldfPath)
	{
		if (!exists(ldfPath))
		{
			throw runtime_error("LinDatabase::loadLdfDatabase: Ldf file '" + ldfPath.string() + "' does not exist.");
		}
		ifstream file(ldfPath, ios::binary);
		if (!file.is_open())
		{
			throw runtime_error("LinDatabase::loadLdfDatabase: Ldf file '" + ldfPath.string() + "' could not be opened.");
		}
		// LdfParser parser(&file);
		// parser.parse(this->_ldfDatabase);
	}

}
