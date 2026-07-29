
#include <OptiScan/Core/Database/SystemDatabase.h>

#include <OptiScan/Core/Database/SelectionTable.h>

using namespace OptiScan::Core::Config;
using namespace std;

namespace OptiScan::Core::Database
{

	SystemDatabase::SystemDatabase()
		: _configDatabase()
		, _logHandler(nullptr)
		, _systemConfig()
		, _systemPath()
	{
	}

	void SystemDatabase::loadCanSelectionTableFromJson(const filesystem::path & selectionTablePath)
	{
		if (!filesystem::exists(selectionTablePath))
		{
			throw runtime_error("SystemDatabase::loadSelectionTable: Selection table file '" + selectionTablePath.string() + "' does not exist.");
		}
		SelectionTable selectionTable;
		selectionTable.loadJsonFile(selectionTablePath);
		selectionTable.parse();
	}

	void SystemDatabase::loadLinSelectionTableFromJson(const filesystem::path & selectionTablePath)
	{

		if (!filesystem::exists(selectionTablePath))
		{
			throw runtime_error("SystemDatabase::loadSelectionTable: Selection table file '" + selectionTablePath.string() + "' does not exist.");
		}
	}

	void SystemDatabase::loadFromConfigFile(const string & filePath)
	{
		filesystem::path const path(filePath);
		this->_systemPath = path.parent_path();
		if (this->_logHandler != nullptr)
		{
			this->_systemConfig.setLogHandler(this->_logHandler);
		}
		this->_systemConfig.loadFromFile(filePath);
		this->_configDatabase.clear();
		this->_systemConfig.parse(this->_configDatabase);
	}

	void SystemDatabase::loadSelectionTables()
	{
		if (this->_configDatabase.hasValue())
		{
			try
			{
				if (this->_configDatabase._canObject.has_value())
				{
					if (this->_configDatabase._canObject->_selectionTable.has_value())
					{
						filesystem::path canSelectionTablePath(this->_systemPath);
						canSelectionTablePath.append(this->_configDatabase._canObject->_selectionTable.value());
						if (!canSelectionTablePath.has_extension())
						{
							canSelectionTablePath += ".json";
						}
						this->loadCanSelectionTableFromJson(canSelectionTablePath);
					}
					if (this->_configDatabase._canObject->_xcpSelectionTable.has_value())
					{
						filesystem::path xcpSelectionTablePath(this->_systemPath);
						xcpSelectionTablePath.append(this->_configDatabase._canObject->_selectionTable.value());
						if (!xcpSelectionTablePath.has_extension())
						{
							xcpSelectionTablePath += ".json";
						}
						this->loadXcpSelectionTableFromJson(xcpSelectionTablePath);
					}
				}
				if (this->_configDatabase._linObject.has_value())
				{
					filesystem::path linSelectionTablePath(this->_systemPath);
					linSelectionTablePath.append(this->_configDatabase._canObject->_selectionTable.value());
					if (!linSelectionTablePath.has_extension())
					{
						linSelectionTablePath += ".json";
					}
					this->loadLinSelectionTableFromJson(linSelectionTablePath);
				}
			}
			catch (const exception & error)
			{
				this->logError("SystemDatabase::loadSelectionTables: " + string(error.what()));
				throw;
			}
		}
	}

	void SystemDatabase::loadXcpSelectionTableFromJson(const filesystem::path & selectionTablePath)
	{
		if (!filesystem::exists(selectionTablePath))
		{
			throw runtime_error("SystemDatabase::loadSelectionTable: Selection table file '" + selectionTablePath.string() + "' does not exist.");
		}
	}

	void SystemDatabase::logError(const string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->logError(message);
		}
	}

	void SystemDatabase::setLogHandler(LogHandler * logHandler)
	{
		this->_logHandler = logHandler;
	}

}
