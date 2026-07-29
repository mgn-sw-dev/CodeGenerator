
#include <OptiScan/Core/Database/SelectionTable.h>
#include <OptiScan/Core/Json/JsonFile.h>
#include <fstream>

using namespace OptiScan::Core::Json;
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
		try
		{
			JsonFile::loadJsonFile(selectionTablePath, this->_jsonRootObject);
		}
		catch (const exception & error)
		{
			throw;
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
