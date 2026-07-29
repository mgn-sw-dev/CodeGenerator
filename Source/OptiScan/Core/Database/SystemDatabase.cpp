
#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/Core/Database/SelectionTable.h>
#include <OptiScan/Core/Json/JsonFile.h>

using namespace OptiScan::Core::Config;
using namespace OptiScan::Core::Json;
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
						filesystem::path canSelectionTablePath;
						JsonFile::buildJsonPathWithExtension(this->_systemPath, this->_configDatabase._canObject->_selectionTable.value(), canSelectionTablePath);
						SelectionTable selectionTable;
						selectionTable.loadJsonFile(canSelectionTablePath);
						selectionTable.parse();
					}
					if (this->_configDatabase._canObject->_xcpSelectionTable.has_value())
					{
						filesystem::path xcpSelectionTablePath;
						JsonFile::buildJsonPathWithExtension(this->_systemPath, this->_configDatabase._canObject->_xcpSelectionTable.value(), xcpSelectionTablePath);
					}
				}
				if (this->_configDatabase._linObject.has_value())
				{
					filesystem::path linSelectionTablePath;
					JsonFile::buildJsonPathWithExtension(this->_systemPath, this->_configDatabase._canObject->_xcpSelectionTable.value(), linSelectionTablePath);
				}
			}
			catch (const exception & error)
			{
				this->logError("SystemDatabase::loadSelectionTables: " + string(error.what()));
				throw;
			}
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
