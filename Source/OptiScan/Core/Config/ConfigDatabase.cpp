
#include <OptiScan/Core/Config/ConfigDatabase.h>

using namespace OptiScan::Core::Common;
using namespace std;

namespace OptiScan::Core::Config
{
	ConfigCanObject::ConfigCanObject()
		: _busses()
		, _frequencyMax_Hz()
		, _selectionTable()
		, _standardTrace()
		, _xcpFrequencyMax_Hz()
		, _xcpSelectionTable()
	{
	}

	void ConfigCanObject::clear()
	{
		this->_busses.clear();
		this->_frequencyMax_Hz.reset();
		this->_selectionTable.reset();
		this->_standardTrace.reset();
		this->_xcpFrequencyMax_Hz.reset();
		this->_xcpSelectionTable.reset();
	}

	ConfigDatabase::ConfigDatabase()
		: _canObject()
		, _debugObject()
		, _gpsObject()
		, _linObject()
		, _projectInfos()
	{
	}

	void ConfigDatabase::clear()
	{
		this->_canObject.reset();
		this->_debugObject.clear();
		this->_gpsObject.clear();
		this->_linObject.reset();
		this->_projectInfos.clear();

	}

	void ConfigDebugObject::clear()
	{
		this->_frequency_Hz = 0.0;
		this->_vinIncluded = false;
	}

	void ConfigGpsObject::clear()
	{
		this->_frequency_Hz = 0.0;
	}

	ConfigLinObject::ConfigLinObject()
		: _busses()
		, _selectionTable()
	{
	}

	void ConfigLinObject::clear()
	{
		this->_busses.clear();
		this->_selectionTable.clear();
	}

	ConfigProjectInfos::ConfigProjectInfos()
		: _customer()
		, _firmwareVersion()
		, _fleetManagementRelease()
		, _generation(0)
		, _projectName()
		, _systemName()
		, _systemVersion()
	{
	}

	void ConfigProjectInfos::clear()
	{
		this->_customer.clear();
		this->_firmwareVersion = Version();
		this->_fleetManagementRelease.clear();
		this->_generation = 0;
		this->_projectName.clear();
		this->_systemName.clear();
		this->_systemVersion = Version();
	}
}
