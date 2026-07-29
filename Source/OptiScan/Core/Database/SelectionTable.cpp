
#include <OptiScan/Core/Database/SelectionTable.h>
#include <OptiScan/Core/Json/JsonFile.h>

using namespace OptiScan::Core::Json;
using namespace nlohmann;
using namespace std;

namespace OptiScan::Core::Database
{
	SelectionTable::SelectionTable()
		: _selectedSignals()
	{
	}

	void SelectionTable::clear()
	{
		this->_selectedSignals.clear();
	}

	bool SelectionTable::hasValue() const
	{
		return !this->_selectedSignals.empty();
	}

	void SelectionTable::loadJsonFile(const filesystem::path & selectionTablePath)
	{
		this->clear();
		JsonFile::loadJsonFile(selectionTablePath, this->_jsonRootObject);
	}

	void SelectionTable::removeFileExtension(const string & fileName, string & result)
	{
		size_t pos = fileName.find_last_of(".");
		if (pos != string::npos)
		{
			result = fileName.substr(0, pos);
		}
	}

	CanSelectionTable::CanSelectionTable()
		: SelectionTable()
	{
	}

	void CanSelectionTable::parse()
	{
		this->clear();

		if (!this->_jsonRootObject.is_null())
		{
			if (!this->_jsonRootObject.is_array())
			{
				throw runtime_error("CanSelectionTable::parse: Selection table JSON is not an array.");
			}

			for (const auto & jsonSignal : this->_jsonRootObject)
			{
				SelectedSignal signal = SelectedSignal();
				signal._signalType = SelectedSignalType::Can;
				signal._signalName = jsonSignal["signalName"].get<string>();
				signal._sampleFrequency_Hz = jsonSignal["sampleRateInHz"].get<double>();
				if (jsonSignal.contains("DisplayName"))
				{
					signal._signalName = jsonSignal["DisplayName"].get<string>();
				}
				else
				{
					signal._displayName = signal._signalName;
				}
				const json canDbcObject = jsonSignal.at("canDbc");
				SelectionTable::removeFileExtension(canDbcObject["fileName"].get<string>(), signal._dbcName);

				const json canMessageObject = jsonSignal.at("canMessage");
				signal._messageName = canMessageObject["name"].get<string>();
				signal._messageId = canMessageObject["frameId"].get<string>();

				this->_selectedSignals.push_back(signal);
			}
		}
	}

	LinSelectionTable::LinSelectionTable()
		: SelectionTable()
	{
	}

	void LinSelectionTable::parse()
	{
		this->clear();
		if (!this->_jsonRootObject.is_null())
		{
			if (!this->_jsonRootObject.is_array())
			{
				throw runtime_error("LinSelectionTable::parse: Selection table JSON is not an array.");
			}
			for (const auto & jsonSignal : this->_jsonRootObject)
			{
				SelectedSignal signal = SelectedSignal();
				signal._signalType = SelectedSignalType::Lin;
				signal._signalName = jsonSignal["signalName"].get<string>();
				signal._sampleFrequency_Hz = jsonSignal["sampleRateInHz"].get<double>();
				if (jsonSignal.contains("DisplayName"))
				{
					signal._signalName = jsonSignal["DisplayName"].get<string>();
				}
				else
				{
					signal._displayName = signal._signalName;
				}

				const json linLdfObject = jsonSignal.at("linLdf");
				SelectionTable::removeFileExtension(linLdfObject["fileName"].get<string>(), signal._ldfName);

				const json linFrameObject = jsonSignal.at("linFrame");
				signal._frameId = to_string(linFrameObject["id"].get<uint32_t>());
				signal._frameName = linFrameObject["name"].get<string>();

				this->_selectedSignals.push_back(signal);
			}
		}
	}

	XcpSelectionTable::XcpSelectionTable()
		: SelectionTable()
	{
	}

	void XcpSelectionTable::parse()
	{
		this->clear();
		if (!this->_jsonRootObject.is_null())
		{
			if (!this->_jsonRootObject.is_array())
			{
				throw runtime_error("XcpSelectionTable::parse: Selection table JSON is not an array.");
			}
			// ToDo
		}
	}
}
