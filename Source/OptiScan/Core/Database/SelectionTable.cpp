
#include <OptiScan/Core/Database/SelectionTable.h>
#include <OptiScan/Core/Database/SelectionTableConstants.h>
#include <OptiScan/Core/Json/JsonFile.h>

using namespace OptiScan::Core::Json;
using namespace nlohmann;
using namespace std;
using namespace std::filesystem;

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
				signal._signalName = jsonSignal[SelectionTableConstants::SignalName].get<string>();
				signal._sampleFrequency_Hz = jsonSignal[SelectionTableConstants::SampleRateInHz].get<double>();
				if (jsonSignal.contains(SelectionTableConstants::DisplayName))
				{
					signal._displayName = jsonSignal[SelectionTableConstants::DisplayName].get<string>();
				}
				else
				{
					signal._displayName = signal._signalName;
				}
				const json canDbcObject = jsonSignal.at(SelectionTableConstants::CanDbc);
				signal._dbcName = path(canDbcObject[SelectionTableConstants::FileName].get<string>()).stem().string();

				const json canMessageObject = jsonSignal.at(SelectionTableConstants::CanMessage);
				signal._messageName = canMessageObject[SelectionTableConstants::Name].get<string>();
				signal._messageId = canMessageObject[SelectionTableConstants::FrameId].get<string>();

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
				signal._signalName = jsonSignal[SelectionTableConstants::SignalName].get<string>();
				signal._sampleFrequency_Hz = jsonSignal[SelectionTableConstants::SampleRateInHz].get<double>();
				if (jsonSignal.contains(SelectionTableConstants::DisplayName))
				{
					signal._displayName = jsonSignal[SelectionTableConstants::DisplayName].get<string>();
				}
				else
				{
					signal._displayName = signal._signalName;
				}

				const json linLdfObject = jsonSignal.at(SelectionTableConstants::LinLdf);
				signal._ldfName = path(linLdfObject[SelectionTableConstants::FileName].get<string>()).stem().string();

				const json linFrameObject = jsonSignal.at(SelectionTableConstants::LinFrame);
				signal._frameId = to_string(linFrameObject[SelectionTableConstants::Id].get<uint32_t>());
				signal._frameName = linFrameObject[SelectionTableConstants::Name].get<string>();

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
