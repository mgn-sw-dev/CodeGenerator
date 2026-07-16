
#include <OptiScan/Core/Database/SelectionTable.h>
#include <fstream>

using namespace nlohmann;
using namespace std;

namespace OptiScan::Core::Database
{
	SelectionTable::SelectionTable()
		: _selectionTable()
	{
	}

	void SelectionTable::loadJsonFile(const filesystem::path & selectionTablePath)
	{
		this->_selectionTable.clear();
		if (!filesystem::exists(selectionTablePath))
		{
			throw runtime_error("CanSelectionTable::loadFromFile: Selection table file '" + selectionTablePath.string() + "' does not exist.");
		}
		this->loadFromJson(selectionTablePath.string(), this->_jsonRootObject);
	}

	void SelectionTable::loadFromJson(const std::string & filePath, json & jsonRootObject)
	{
		ifstream file(filePath);
		if (!file.is_open())
		{
			throw runtime_error("SystemConfig::loadConfigFile:Failed to open file: " + filePath);
		}
		try
		{
			file >> jsonRootObject;
		}
		catch (const json::parse_error & error)
		{
			throw runtime_error("SystemConfig::loadConfigFile: JSON Parse Error: " + string(error.what()));
		}
	}

	void SelectionTable::clear()
	{
		this->_selectionTable.clear();
	}

	void CanSelectionTable::parse()
	{

	}
}
