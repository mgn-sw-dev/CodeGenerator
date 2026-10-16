
#include <OptiScan/Core/Database/XcpDatabase.h>
#include <OptiScan/Parser/A2l/A2lParser.h>
#include <OptiScan/Parser/A2l/IfData/Xcp/XcpIfDataParser.h>
#include <fstream>
#include <memory>

using namespace OptiScan::Parser::A2l;
using namespace std;
using namespace std::filesystem;

namespace OptiScan::Core::Database
{
	XcpDatabase::XcpDatabase()
		: _a2lDatabase()
		, _name()
	{
	}

	void XcpDatabase::clear()
	{
		this->_a2lDatabase = A2lDatabase();
		this->_name.clear();
	}

	void XcpDatabase::loadA2lDatabase(const path & a2lPath)
	{
		if (!exists(a2lPath))
		{
			throw runtime_error("XcpDatabase::loadA2lDatabase: A2l file '" + a2lPath.string() + "' does not exist.");
		}
		ifstream file(a2lPath, ios::binary);
		if (!file.is_open())
		{
			throw runtime_error("XcpDatabase::loadA2lDatabase: A2l file '" + a2lPath.string() + "' could not be opened.");
		}
		A2lParser parser(&file);
		// Without a registered IF_DATA parser every IF_DATA block is skipped, so the
		// XCP part of the A2L would never reach the model. Both flavours are needed:
		// Config::CanBusType knows Xcp and XcpPlus.
		parser.addIfDataParser(make_shared<IfData::Xcp::XcpIfDataParser>());
		parser.addIfDataParser(make_shared<IfData::Xcp::XcpPlusIfDataParser>());
		parser.parse(this->_a2lDatabase);
	}
}
