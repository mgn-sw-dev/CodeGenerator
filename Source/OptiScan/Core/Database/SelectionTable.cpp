
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

	void SelectionTable::clear()
	{
		this->_selectionTable.clear();
	}

	void SelectionTable::loadJsonFile(const filesystem::path & selectionTablePath)
	{
		this->_selectionTable.clear();
		if (!filesystem::exists(selectionTablePath))
		{
			throw runtime_error("CanSelectionTable::loadFromFile: Selection table file '" + selectionTablePath.string() + "' does not exist.");
		}
		try
		{
			this->loadFromJson(selectionTablePath.string(), this->_jsonRootObject);
		}
		catch (const exception & error)
		{
			throw;
		}
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

	void SelectionTable::parse()
	{
		this->_selectionTable.clear();

		if (!this->_jsonRootObject.is_null())
		{
			if (!this->_jsonRootObject.is_array())
			{
				throw runtime_error("SelectionTable::parse: Selection table JSON is not an array.");
			}

			for (const auto & jsonSignal : this->_jsonRootObject)
			{
				SelectedSignal signal = SelectedSignal();
				signal._signalName = jsonSignal["signalName"];
				signal._sampleFrequency_Hz = jsonSignal["sampleRateInHz"];
				signal._signalType = SelectedSignalType::Can;
				this->_selectionTable.push_back(signal);
			}
		}
	}

}
