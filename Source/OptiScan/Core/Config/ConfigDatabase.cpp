
#include <OptiScan/Core/Config/ConfigDatabase.h>

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

	ConfigDatabase::ConfigDatabase()
		: _projectInfos()
	{
	}

	ConfigProjectInfos::ConfigProjectInfos()
		: _customer()
		, _firmwareVersion()
		, _fleetManagementRelease()
		, _generation()
		, _projectName()
		, _systemName()
		, _systemVersion()
	{
	}
}
