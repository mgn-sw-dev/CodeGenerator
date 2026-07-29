
#include <OptiScan/Core/Database/SystemDatabase.h>
#include <OptiScan/Core/Json/JsonFile.h>

using namespace OptiScan::Core::Config;
using namespace OptiScan::Core::Json;
using namespace std;

namespace OptiScan::Core::Database
{

	SystemDatabase::SystemDatabase()
		: _canDatabases()
		, _canSelectionTable()
		, _configDatabase()
		, _linSelectionTable()
		, _logHandler(nullptr)
		, _mapping()
		, _systemConfig()
		, _systemPath()
		, _xcpSelectionTable()
	{
	}

	void SystemDatabase::loadMapping()
	{
		if (this->_configDatabase.hasValue())
		{
			this->_mapping.clear();
			if (this->_configDatabase._canObject.has_value())
			{
				for (const CanBusObject & canBus : this->_configDatabase._canObject.value()._busses)
				{
					MappingCanObject mapping;
					mapping._busName = canBus._name;
					switch (canBus._type)
					{
					case CanBusType::Standard:
					case CanBusType::Fd:
						mapping._vmmNames = canBus._dbcNames;
						break;
					case CanBusType::Xcp:
					case CanBusType::XcpPlus:
						mapping._vmmNames = { canBus._a2lName };
						break;
					case CanBusType::VoiceToCan:
					case CanBusType::Unknown:
					default:
						break;
					}
					this->_mapping.setCanMapping(canBus._hardwareId, mapping);
				}
			}
			if (this->_configDatabase._linObject.has_value())
			{
				for (const LinBusObject & linBus : this->_configDatabase._linObject.value()._busses)
				{
					MappingLinObject mapping;
					mapping._busName = linBus._name;
					mapping._ldfName = linBus._ldfName;
					this->_mapping.setLinMapping(linBus._hardwareId, mapping);
				}
			}
			this->logInfo("System mapping loaded");
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
						filesystem::path canSelectionTablePath;
						JsonFile::buildJsonPathWithExtension(this->_systemPath, this->_configDatabase._canObject->_selectionTable.value(), canSelectionTablePath);
						this->_canSelectionTable.loadJsonFile(canSelectionTablePath);
						this->_canSelectionTable.parse();
						this->logInfo("Load Can selection table: " + canSelectionTablePath.string()
							+ "\n\t- SignalsCount: " + to_string(this->_canSelectionTable._selectedSignals.size()));
					}
					if (this->_configDatabase._canObject->_xcpSelectionTable.has_value())
					{
						filesystem::path xcpSelectionTablePath;
						JsonFile::buildJsonPathWithExtension(this->_systemPath, this->_configDatabase._canObject->_xcpSelectionTable.value(), xcpSelectionTablePath);
						this->_xcpSelectionTable.loadJsonFile(xcpSelectionTablePath);
						this->_xcpSelectionTable.parse();
						this->logInfo("Load XCP selection table: " + xcpSelectionTablePath.string()
							+"\n\t- SignalsCount: " + to_string(this->_xcpSelectionTable._selectedSignals.size()));
					}
				}
				if (this->_configDatabase._linObject.has_value())
				{
					filesystem::path linSelectionTablePath;
					JsonFile::buildJsonPathWithExtension(this->_systemPath, this->_configDatabase._linObject->_selectionTable, linSelectionTablePath);
					 this->_linSelectionTable.loadJsonFile(linSelectionTablePath);
					 this->_linSelectionTable.parse();
					this->logInfo("Load Lin selection table: " + linSelectionTablePath.string()
						+ "\n\t- SignalsCount: " + to_string(this->_linSelectionTable._selectedSignals.size()));
				}
			}
			catch (const exception & error)
			{
				this->logError("SystemDatabase::loadSelectionTables: " + string(error.what()));
				throw;
			}
		}
	}

	void SystemDatabase::loadVmms()
	{
		if (this->_configDatabase.hasValue())
		{
			if (this->_configDatabase._canObject.has_value())
			{
				for (const CanBusObject & canBus : this->_configDatabase._canObject.value()._busses)
				{
					if (canBus._type == CanBusType::Standard || canBus._type == CanBusType::Fd)
					{
						this->logInfo("Load CanBus " + to_string(canBus._hardwareId));
						for (const string & dbcName : canBus._dbcNames)
						{
							this->log("\t- DBC: " + dbcName);
							CanDatabase canDatabase = CanDatabase();
							canDatabase._name = dbcName;
							filesystem::path dbcPath = this->_systemPath / dbcName += ".dbc";
							try
							{
								canDatabase.loadDbcDatabase(dbcPath);
							}
							catch (const exception & error)
							{
								this->logError(string(error.what()));
								throw;
							}
							this->_canDatabases.push_back(canDatabase);
						}
					}
				}
			}
		}
	}

	void SystemDatabase::log(const std::string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->log(message);
		}
	}

	void SystemDatabase::logError(const string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->logError(message);
		}
	}

	void SystemDatabase::logInfo(const std::string & message) const
	{
		if (this->_logHandler)
		{
			this->_logHandler->logInfo(message);
		}
	}

	void SystemDatabase::setLogHandler(LogHandler * logHandler)
	{
		this->_logHandler = logHandler;
	}

}
